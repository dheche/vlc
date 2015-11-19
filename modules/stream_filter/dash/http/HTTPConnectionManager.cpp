/*
 * HTTPConnectionManager.cpp
 *****************************************************************************
 * Copyright (C) 2010 - 2011 Klagenfurt University
 *
 * Created on: Aug 10, 2010
 * Authors: Christopher Mueller <christopher.mueller@itec.uni-klu.ac.at>
 *          Christian Timmerer  <christian.timmerer@itec.uni-klu.ac.at>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <sys/time.h>
#include "HTTPConnectionManager.h"
#include "mpd/Segment.h"

#define OML_FROM_MAIN
#include "../../vlc-dash_oml2.h"
#include <stddef.h>
#include <oml2/omlc.h>

using namespace dash::http;
using namespace dash::logic;

extern oml_mps_t* g_oml_mps_vlc2;

struct timeval tv;

const size_t    HTTPConnectionManager::PIPELINE               = 80;
const size_t    HTTPConnectionManager::PIPELINELENGTH         = 2;
const uint64_t  HTTPConnectionManager::CHUNKDEFAULTBITRATE    = 1;

HTTPConnectionManager::HTTPConnectionManager    (logic::IAdaptationLogic *adaptationLogic, stream_t *stream) :
                       adaptationLogic          (adaptationLogic),
                       stream                   (stream),
                       chunkCount               (0),
                       bpsAvg                   (0),
                       bpsLastChunk             (0),
                       bpsCurrentChunk          (0),
                       bytesReadSession         (0),
                       bytesReadChunk           (0),
                       timeSession              (0),
                       timeChunk                (0)
{
}
HTTPConnectionManager::~HTTPConnectionManager   ()
{
    this->closeAllConnections();
}

void                                HTTPConnectionManager::closeAllConnections      ()
{
    vlc_delete_all(this->connectionPool);
    vlc_delete_all(this->downloadQueue);
}
int                                 HTTPConnectionManager::read                     (block_t *block)
{
   
   if (this->timeSession==0) {
    oml_register_mps();


   int result = omlc_start();

   if (result == -1) {
     fprintf (stderr, "Error starting up OML measurement streams\n");
     exit (1);
   }
  }
  
  
  

    if(this->downloadQueue.size() == 0)
        if(!this->addChunk(this->adaptationLogic->getNextChunk()))
            return 0;

    if(this->downloadQueue.front()->getPercentDownloaded() > HTTPConnectionManager::PIPELINE &&
       this->downloadQueue.size() < HTTPConnectionManager::PIPELINELENGTH)
        this->addChunk(this->adaptationLogic->getNextChunk());

    int ret = 0;

    mtime_t start = mdate();
    ret = this->downloadQueue.front()->getConnection()->read(block->p_buffer, block->i_buffer);
    mtime_t end = mdate();

    block->i_length = (mtime_t)((ret * 8) / ((float)this->downloadQueue.front()->getBitrate() / 1000000));

    double time = ((double)(end - start)) / 1000000;

    if(ret <= 0)
    {

        this->bpsLastChunk   = this->bpsCurrentChunk;
        this->bytesReadChunk = 0;
        this->timeChunk      = 0;

        delete(this->downloadQueue.front());
        this->downloadQueue.pop_front();

        return this->read(block);
    }
    else
    {
        this->updateStatistics(ret, time);
    }

    return ret;
}
void                                HTTPConnectionManager::attach                   (IDownloadRateObserver *observer)
{
    this->rateObservers.push_back(observer);
}
void                                HTTPConnectionManager::notify                   ()
{
    if ( this->bpsAvg == 0 )
        return ;
    for(size_t i = 0; i < this->rateObservers.size(); i++)
        this->rateObservers.at(i)->downloadRateChanged(this->bpsAvg, this->bpsLastChunk);
}
std::vector<PersistentConnection *> HTTPConnectionManager::getConnectionsForHost    (const std::string &hostname)
{
    std::vector<PersistentConnection *> cons;

    for(size_t i = 0; i < this->connectionPool.size(); i++)
        if(!this->connectionPool.at(i)->getHostname().compare(hostname) || !this->connectionPool.at(i)->isConnected())
            cons.push_back(this->connectionPool.at(i));

    return cons;
}
void                                HTTPConnectionManager::updateStatistics         (int bytes, double time)
{
    this->bytesReadSession  += bytes;
    this->bytesReadChunk    += bytes;
    this->timeSession       += time;
    this->timeChunk         += time;

    this->bpsAvg            = (int64_t) ((this->bytesReadSession * 8) / this->timeSession);
    this->bpsCurrentChunk   = (int64_t) ((this->bytesReadChunk * 8) / this->timeChunk);

    
    oml_inject_dashDlSession(g_oml_mps_vlc2->dashDlSession, 
    	(int32_t) this->chunkCount,
        (int32_t) this->bytesReadSession, 
    	(int32_t) this->bytesReadChunk,
        (double) this->timeSession,
        (double) this->timeChunk);
   gettimeofday(&tv,NULL);
    /*
    fprintf(stderr,"HTTPConnection\t%f\t%ld\t\t%ld\t%d\t%f\t%f\n",
      tv.tv_sec + tv.tv_usec/1000000.0,
      this->chunkCount,
      this->bytesReadSession,
      this->bytesReadChunk,
      this->timeSession,
      this->timeChunk);
    */

    if(this->bpsAvg < 0)
        this->bpsAvg = 0;

    if(this->bpsCurrentChunk < 0)
        this->bpsCurrentChunk = 0;

    this->notify();
}
bool                                HTTPConnectionManager::addChunk                 (Chunk *chunk)
{
    if(chunk == NULL)
        return false;

    this->downloadQueue.push_back(chunk);

    std::vector<PersistentConnection *> cons = this->getConnectionsForHost(chunk->getHostname());

    if(cons.size() == 0)
    {
        PersistentConnection *con = new PersistentConnection(this->stream);
        this->connectionPool.push_back(con);
        cons.push_back(con);
    }

    size_t pos = this->chunkCount % cons.size();

    cons.at(pos)->addChunk(chunk);

    chunk->setConnection(cons.at(pos));

    this->chunkCount++;

    if(chunk->getBitrate() <= 0)
        chunk->setBitrate(HTTPConnectionManager::CHUNKDEFAULTBITRATE);

    return true;
}
