/*******************************************************************************
 * paintchatplugin.h                                                           *
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

#ifndef PAINTCHATPLUGIN_H
#define PAINTCHATPLUGIN_H

#include <retroshare/rsplugin.h>

class PaintChatPlugin : public RsPlugin
{
public:
    PaintChatPlugin();
    virtual ~PaintChatPlugin();

	virtual void getPluginVersion(int& major,int& minor, int& build, int& svn_rev) const ;
    virtual void setPlugInHandler(RsPluginHandler *pgHandler);


    virtual std::string getShortPluginDescription() const ;
    virtual std::string getPluginName() const;
    virtual void setInterfaces(RsPlugInInterfaces& interfaces);

    virtual RsPQIService   *rs_pqi_service() 			const	;
    virtual uint16_t        rs_service_id()         const { return 0x255 ; }

    virtual ChatWidgetHolder *qt_get_chat_widget_holder(ChatWidget *chatWidget) const;

    virtual QIcon *qt_icon() const;

private:
    RsPluginHandler *mPlugInHandler;
    // keine ahnung warum mutable
    mutable QIcon *mIcon;
};

#endif // PAINTCHATPLUGIN_H
