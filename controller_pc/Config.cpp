/*
 * BCFlight
 * Copyright (C) 2016 Adrien Aubry (drich)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/wireless.h>
#include <ifaddrs.h>
#include <QtCore/QDebug>
#include "Config.h"
#include "ui_config.h"

Config::Config()
	: QDialog()
	, mSettings( new QSettings( "BeyondChaos", "Flight" ) )
{
	ui = new Ui::Config;
	ui->setupUi(this);
	connect( this, SIGNAL( accepted() ), this, SLOT( Save() ) );

#ifdef WIN32
#else
	struct ifaddrs *ifa, *ifap;
	getifaddrs( &ifap );
	ifa = ifap;
	while ( ifa != nullptr ) {
//     if ( ifa->ifa_addr == NULL or ifa->ifa_addr->sa_family != AF_PACKET ) continue;
		bool iswifi = false;
		int sock = -1;
		struct iwreq pwrq;
		memset( &pwrq, 0, sizeof(pwrq) );
		strncpy( pwrq.ifr_name, ifa->ifa_name, IFNAMSIZ );
		sock = socket( AF_INET, SOCK_STREAM, 0 );
		if ( ioctl( sock, SIOCGIWNAME, &pwrq ) != -1 ) {
			iswifi = true;
		}
		::close(sock);
		if ( iswifi ) {
			ui->rawwifi_device->addItem( ifa->ifa_name );
		}
		ifa = ifa->ifa_next;
	}
	freeifaddrs( ifap );
#endif

	// Set controller settings
	ui->spectate->setChecked( value( "controller/spectate", false ).toBool() );

	// Set connection settings
	ui->rawwifi->setChecked( value( "link/rawwifi", true ).toBool() );
	ui->tcpip->setChecked( value( "link/tcpip", false ).toBool() );
	// Set RawWifi settings
	ui->rawwifi_device->setCurrentText( value( "rawwifi/device", ui->rawwifi_device->count() > 0 ? ui->rawwifi_device->itemText(0) : "" ).toString() );
	ui->rawwifi_channel->setCurrentIndex( value( "rawwifi/channel", 9 ).toInt() - 1 );
	ui->rawwifi_controller_inport->setValue( value( "rawwifi/controller/inport", 1 ).toInt() );
	ui->rawwifi_controller_nodrop->setChecked( value( "rawwifi/controller/nodrop", true ).toBool() );
	ui->rawwifi_controller_cec->setCurrentText( value( "rawwifi/controller/cec", "Weighted" ).toString() );
	ui->rawwifi_controller_outport->setValue( value( "rawwifi/controller/outport", 0 ).toInt() );
	ui->rawwifi_controller_retries->setValue( value( "rawwifi/controller/retries", 2 ).toInt() );
	ui->rawwifi_video_inport->setValue( value( "rawwifi/video/inport", 11 ).toInt() );
	ui->rawwifi_video_nodrop->setChecked( value( "rawwifi/video/nodrop", true ).toBool() );
	ui->rawwifi_video_cec->setCurrentText( value( "rawwifi/video/cec", "Weighted" ).toString() );
	ui->rawwifi_video_outport->setValue( value( "rawwifi/video/outport", 10 ).toInt() );
	ui->rawwifi_video_retries->setValue( value( "rawwifi/video/retries", 1 ).toInt() );
	// Set TCP/IP settings
	ui->tcpip_drone_address->setText( value( "tcpip/address", "192.168.32.1" ).toString() );
	ui->tcpip_controller_type->setCurrentText( value( "tcpip/controller/type", "TCP" ).toString() );
	ui->tcpip_controller_port->setValue( value( "tcpip/controller/port", 2020 ).toInt() );
	ui->tcpip_video_type->setCurrentText( value( "tcpip/video/type", "UDPLite" ).toString() );
	ui->tcpip_video_port->setValue( value( "tcpip/video/port", 2021 ).toInt() );
}


Config::~Config()
{
	delete ui;
	delete mSettings;
}


void Config::Save()
{
	setValue( "controller/spectate", ui->spectate->isChecked() );
	setValue( "link/rawwifi", ui->rawwifi->isChecked() );
	setValue( "link/tcpip", ui->tcpip->isChecked() );
	setValue( "rawwifi/device", ui->rawwifi_device->currentText() );
	setValue( "rawwifi/channel", ui->rawwifi_channel->currentIndex() + 1 );
	setValue( "rawwifi/controller/inport", ui->rawwifi_controller_inport->value() );
	setValue( "rawwifi/controller/nodrop", ui->rawwifi_controller_nodrop->isChecked() );
	setValue( "rawwifi/controller/cec", ui->rawwifi_controller_cec->currentText() );
	setValue( "rawwifi/controller/outport", ui->rawwifi_controller_outport->value() );
	setValue( "rawwifi/controller/retries", ui->rawwifi_controller_retries->value() );
	setValue( "rawwifi/video/inport", ui->rawwifi_video_inport->value() );
	setValue( "rawwifi/video/nodrop", ui->rawwifi_video_nodrop->isChecked() );
	setValue( "rawwifi/video/cec", ui->rawwifi_video_cec->currentText() );
	setValue( "rawwifi/video/outport", ui->rawwifi_video_outport->value() );
	setValue( "rawwifi/video/retries", ui->rawwifi_video_retries->value() );
	setValue( "tcpip/address", ui->tcpip_drone_address->text() );
	setValue( "tcpip/controller/type", ui->tcpip_controller_type->currentText() );
	setValue( "tcpip/controller/port", ui->tcpip_controller_port->value() );
	setValue( "tcpip/video/type", ui->tcpip_video_type->currentText() );
	setValue( "tcpip/video/port", ui->tcpip_video_port->value() );
	mSettings->sync();
	qDebug() << "Saved config";
}


void Config::setValue( const QString& key, const QVariant& value )
{
	mSettings->setValue( key, value );
}


QVariant Config::value( const QString& key, const QVariant& defaultValue ) const
{
	return mSettings->value( key, defaultValue );
}