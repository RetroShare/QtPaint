/*******************************************************************************
 * paintchatplugin.cpp                                                         *
 *                                                                             *
 * Copyright (C) 2025 Retroshare Team <retroshare.project@gmail.com>           *
 *                                                                             *
 * This program is free software: you can redistribute it and/or modify        *
 * it under the terms of the GNU Affero General Public License as              *
 * published by the Free Software Foundation, either version 3 of the          *
 * License, or (at your option) any later version.                             *
 *                                                                             *
 * This program is distributed in the hope that it will be useful,             *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                *
 * GNU Affero General Public License for more details.                         *
 *                                                                             *
 * You should have received a copy of the GNU Affero General Public License    *
 * along with this program. If not, see <https://www.gnu.org/licenses/>.       *
 *                                                                             *
 *******************************************************************************/

#include "paintchatplugin.h"

#include "paintchatpopupchatdialog.h"

#define IMAGE_QTPAINT ":/icons/app.png"

extern "C" {

    // This is *the* functions required by RS plugin system to give RS access to the plugin.
    // Be careful to:
    // - always respect the C linkage convention
    // - always return an object of type RsPlugin*
    //
#ifdef WIN32
	__declspec(dllexport)
#endif
    void *RETROSHARE_PLUGIN_provide()
    {
        static PaintChatPlugin *p = new PaintChatPlugin() ;

        return (void*)p ;
    }

    // This symbol contains the svn revision number grabbed from the executable.
    // It will be tested by RS to load the plugin automatically, since it is safe to load plugins
    // with same revision numbers, assuming that the revision numbers are up-to-date.
    //
#ifdef WIN32
	__declspec(dllexport)
#endif
	uint32_t RETROSHARE_PLUGIN_revision = 1 ;

    // This symbol contains the svn revision number grabbed from the executable.
    // It will be tested by RS to load the plugin automatically, since it is safe to load plugins
    // with same revision numbers, assuming that the revision numbers are up-to-date.
    //
#ifdef WIN32
	__declspec(dllexport)
#endif
    uint32_t RETROSHARE_PLUGIN_api = RS_PLUGIN_API_VERSION ;
}

PaintChatPlugin::PaintChatPlugin(): mIcon(NULL) {
    std::cerr<<"PaintChatPlugin::PaintChatPlugin()"<<std::endl;
}
PaintChatPlugin::~PaintChatPlugin(){
    std::cerr<<"PaintChatPlugin::~PaintChatPlugin()"<<std::endl;
}

std::string PaintChatPlugin::getShortPluginDescription()const{
    return std::string("QtPaint plugin for chat rooms");
}
std::string PaintChatPlugin::getPluginName()const{
    return std::string("QtPaint");
}
void PaintChatPlugin::getPluginVersion(int &major, int &minor, int &build, int &svn_rev)const{
	major = RS_MAJOR_VERSION;
	minor = RS_MINOR_VERSION;
	build = RS_MINI_VERSION;
	svn_rev = 1;
}
void PaintChatPlugin::setInterfaces(RsPlugInInterfaces &interfaces){

}
void PaintChatPlugin::setPlugInHandler(RsPluginHandler *pgHandler){
    std::cerr<<"PaintChatPlugin::setPlugInHandler()"<<std::endl;
    mPlugInHandler = pgHandler;
}

RsPQIService *PaintChatPlugin::rs_pqi_service()const{
//    std::cerr<<"PaintChatPlugin::rs_pqi_service()"<<std::endl;
//    if(mService==NULL){
//        std::cerr<<"PaintChatPlugin::rs_pqi_service(): creating new p3PaintChatService"<<std::endl;
//        mService=new p3PaintChatService(mPlugInHandler);
//        paintChatService=mService;
//    }
	return nullptr;
}

ChatWidgetHolder *PaintChatPlugin::qt_get_chat_widget_holder(ChatWidget *chatWidget) const{
    switch (chatWidget->chatType()) {
    case ChatWidget::CHATTYPE_LOBBY:
         return new PaintChatPopupChatDialog(chatWidget);
    case ChatWidget::CHATTYPE_PRIVATE:
    case ChatWidget::CHATTYPE_UNKNOWN:
    case ChatWidget::CHATTYPE_DISTANT:
        break;
    }
    return NULL;
}

QIcon *PaintChatPlugin::qt_icon() const
{
    if (mIcon == NULL) {
        //Q_INIT_RESOURCE(paintchat_images);

        mIcon = new QIcon(IMAGE_QTPAINT);
    }

    return mIcon;
}
