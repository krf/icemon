/*
    This file is part of Icecream.

    Copyright (c) 2004 Andre Wöbbeking <Woebbeking@web.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "detailedhostview.h"

#include "hostinfo.h"
#include "hostlistview.h"
#include "listview.h"

#include <services/logging.h>

#include <klocale.h>
#include <kdebug.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qsplitter.h>

#include <sys/utsname.h>


static QString myHostName()
{
    struct utsname uname_buf;
    if ( ::uname( &uname_buf ) == 0 )
        return uname_buf.nodename;
    else
        return QString::null;
}


DetailedHostView::DetailedHostView( HostInfoManager* manager,
                                    QWidget* parent,
                                    const char* name )
    : QWidget( parent, name ),
      StatusView( manager )
{
  QBoxLayout* topLayout = new QVBoxLayout( this );

  QSplitter* viewSplitter = new QSplitter( Qt::Vertical, this );
  topLayout->addWidget( viewSplitter );

  mHostListView = new HostListView( manager, viewSplitter, "HostListView" );
  mLocalJobsView = new ListStatusView( manager, viewSplitter, "LocalJobs" );
  mRemoteJobsView = new ListStatusView( manager, viewSplitter, "RemoteJobs" );

  connect(mHostListView, SIGNAL( nodeActivated( unsigned int ) ),
          this, SLOT( slotNodeActivated() ) );
}


void DetailedHostView::update( const Job &job )
{
#if 0
    kdDebug() << "DetailedHostView::update() " << job.jobId()
              << " server: " << job.server() << " client: " << job.client()
              << " state: " << job.stateAsString() << endl;
#endif

    const unsigned int hostid = mHostListView->activeNode();

    if ( !hostid )
        return;

    if ( job.client() != hostid && job.server() != hostid ) return;

    if ( job.client() == hostid )
        mLocalJobsView->update( job );
    else
        mRemoteJobsView->update( job );

    const bool finished = job.state() == Job::Finished || job.state() == Job::Failed;

    if ( finished )
    {
        if ( job.client() == hostid )
            mLocalJobsView->removeJob( job );
        else
            mRemoteJobsView->removeJob( job );
    }
}


void DetailedHostView::checkNode( unsigned int hostid )
{
    if ( !hostid )
        return;

    mHostListView->checkNode( hostid );

//  kdDebug() << "DetailedHostView::checkNode(): " << hostid << endl;

    const unsigned int activeNode = mHostListView->activeNode();

    if ( !activeNode )
    {
        HostInfo* info = hostInfoManager()->find( hostid );
        if ( info->name() == myHostName() )
            mHostListView->setActiveNode( hostid );
    }
}


void DetailedHostView::removeNode( unsigned int hostid )
{
    kdDebug() << "DetailedHostView::removeNode(): " << hostid << endl;

    mHostListView->removeNode( hostid );
}


void DetailedHostView::updateSchedulerState( bool online )
{
    if ( online )
    {
    }
    else
    {
    }
}


void DetailedHostView::slotNodeActivated()
{
    mLocalJobsView->clear();
    mRemoteJobsView->clear();
}


QWidget* DetailedHostView::widget()
{
  return this;
}


#include "detailedhostview.moc"