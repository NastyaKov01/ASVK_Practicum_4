/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */


#include "ListenerHelper.h"
#include "MyProtocolHandler.h"

#include <com/sun/star/awt/MessageBoxButtons.hpp>
#include <com/sun/star/awt/Toolkit.hpp>
#include <com/sun/star/awt/XMessageBoxFactory.hpp>
#include <com/sun/star/frame/ControlCommand.hpp>
#include <com/sun/star/text/XTextViewCursorSupplier.hpp>
#include <com/sun/star/sheet/XSpreadsheetView.hpp>
#include <com/sun/star/system/SystemShellExecute.hpp>
#include <com/sun/star/system/SystemShellExecuteFlags.hpp>
#include <com/sun/star/system/XSystemShellExecute.hpp>
#include <cppuhelper/supportsservice.hxx>
#include <com/sun/star/frame/XComponentLoader.hpp>
#include <com/sun/star/text/XTextDocument.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/frame/XLoaderFactory.hpp>
#include <com/sun/star/text/XTextTable.hpp>
#include <com/sun/star/table/XCell.hpp>

#include <map>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace com::sun::star::awt;
using namespace com::sun::star::frame;
using namespace com::sun::star::system;
using namespace com::sun::star::uno;

using com::sun::star::beans::NamedValue;
using com::sun::star::beans::PropertyValue;
using com::sun::star::sheet::XSpreadsheetView;
using com::sun::star::text::XTextViewCursorSupplier;
using com::sun::star::util::URL;

ListenerHelper aListenerHelper;

void BaseDispatch::ShowMessageBox( const Reference< XFrame >& rFrame, const ::rtl::OUString& aTitle, const ::rtl::OUString& aMsgText )
{
    if ( !mxToolkit.is() )
        mxToolkit = Toolkit::create(mxContext);
    Reference< XMessageBoxFactory > xMsgBoxFactory( mxToolkit, UNO_QUERY );
    if ( rFrame.is() && xMsgBoxFactory.is() )
    {
        Reference< XMessageBox > xMsgBox = xMsgBoxFactory->createMessageBox(
            Reference< XWindowPeer >( rFrame->getContainerWindow(), UNO_QUERY ),
            com::sun::star::awt::MessageBoxType_INFOBOX,
            MessageBoxButtons::BUTTONS_OK,
            aTitle,
            aMsgText );

        if ( xMsgBox.is() )
            xMsgBox->execute();
    }
}

void BaseDispatch::SendCommand( const com::sun::star::util::URL& aURL, const ::rtl::OUString& rCommand, const Sequence< NamedValue >& rArgs, sal_Bool bEnabled )
{
    Reference < XDispatch > xDispatch =
            aListenerHelper.GetDispatch( mxFrame, aURL.Path );

    FeatureStateEvent aEvent;

    aEvent.FeatureURL = aURL;
    aEvent.Source     = xDispatch;
    aEvent.IsEnabled  = bEnabled;
    aEvent.Requery    = sal_False;

    ControlCommand aCtrlCmd;
    aCtrlCmd.Command   = rCommand;
    aCtrlCmd.Arguments = rArgs;

    aEvent.State <<= aCtrlCmd;
    aListenerHelper.Notify( mxFrame, aEvent.FeatureURL.Path, aEvent );
}

void BaseDispatch::SendCommandTo( const Reference< XStatusListener >& xControl, const URL& aURL, const ::rtl::OUString& rCommand, const Sequence< NamedValue >& rArgs, sal_Bool bEnabled )
{
    FeatureStateEvent aEvent;

    aEvent.FeatureURL = aURL;
    aEvent.Source     = (::com::sun::star::frame::XDispatch*) this;
    aEvent.IsEnabled  = bEnabled;
    aEvent.Requery    = sal_False;

    ControlCommand aCtrlCmd;
    aCtrlCmd.Command   = rCommand;
    aCtrlCmd.Arguments = rArgs;

    aEvent.State <<= aCtrlCmd;
    xControl->statusChanged( aEvent );
}

void SAL_CALL MyProtocolHandler::initialize( const Sequence< Any >& aArguments ) throw ( Exception, RuntimeException)
{
    Reference < XFrame > xFrame;
    if ( aArguments.getLength() )
    {
        // the first Argument is always the Frame, as a ProtocolHandler needs to have access
        // to the context in which it is invoked.
        aArguments[0] >>= xFrame;
        mxFrame = xFrame;
    }
}

Reference< XDispatch > SAL_CALL MyProtocolHandler::queryDispatch(   const URL& aURL, const ::rtl::OUString& sTargetFrameName, sal_Int32 nSearchFlags )
                throw( RuntimeException )
{
    Reference < XDispatch > xRet;
    if ( !mxFrame.is() )
        return 0;

    Reference < XController > xCtrl = mxFrame->getController();
    if ( xCtrl.is() && aURL.Protocol == "vnd.demo.complextoolbarcontrols.demoaddon:" )
    {
        Reference < XTextViewCursorSupplier > xCursor( xCtrl, UNO_QUERY );
        Reference < XSpreadsheetView > xView( xCtrl, UNO_QUERY );
        if ( !xCursor.is() && !xView.is() )
            // without an appropriate corresponding document the handler doesn't function
            return xRet;

        if ( aURL.Path == "GenButtonCmd" ||
             aURL.Path == "SpinfieldCmd" ||
             aURL.Path == "DropdownboxCmd" ||
             aURL.Path == "ComboboxCmd" ||
             aURL.Path == "StatButtonCmd" ||
             aURL.Path == "ColorButtonCmd" )
        {
            xRet = aListenerHelper.GetDispatch( mxFrame, aURL.Path );
            if ( !xRet.is() )
            {
                xRet = xCursor.is() ? (BaseDispatch*) new WriterDispatch( mxContext, mxFrame ) :
                    (BaseDispatch*) new CalcDispatch( mxContext, mxFrame );
                aListenerHelper.AddDispatch( xRet, mxFrame, aURL.Path );
            }
        }
    }

    return xRet;
}

Sequence < Reference< XDispatch > > SAL_CALL MyProtocolHandler::queryDispatches( const Sequence < DispatchDescriptor >& seqDescripts )
            throw( RuntimeException )
{
    sal_Int32 nCount = seqDescripts.getLength();
    Sequence < Reference < XDispatch > > lDispatcher( nCount );

    for( sal_Int32 i=0; i<nCount; ++i )
        lDispatcher[i] = queryDispatch( seqDescripts[i].FeatureURL, seqDescripts[i].FrameName, seqDescripts[i].SearchFlags );

    return lDispatcher;
}

::rtl::OUString MyProtocolHandler_getImplementationName ()
    throw (RuntimeException)
{
    return ::rtl::OUString( MYPROTOCOLHANDLER_IMPLEMENTATIONNAME );
}

Sequence< ::rtl::OUString > SAL_CALL MyProtocolHandler_getSupportedServiceNames(  )
    throw (RuntimeException)
{
    Sequence < ::rtl::OUString > aRet(1);
    aRet[0] = ::rtl::OUString( MYPROTOCOLHANDLER_SERVICENAME );
    return aRet;
}

#undef SERVICE_NAME

Reference< XInterface > SAL_CALL MyProtocolHandler_createInstance( const Reference< XComponentContext > & rSMgr)
    throw( Exception )
{
    return (cppu::OWeakObject*) new MyProtocolHandler( rSMgr );
}

// XServiceInfo
::rtl::OUString SAL_CALL MyProtocolHandler::getImplementationName(  )
    throw (RuntimeException)
{
    return MyProtocolHandler_getImplementationName();
}

sal_Bool SAL_CALL MyProtocolHandler::supportsService( const ::rtl::OUString& rServiceName )
    throw (RuntimeException)
{
    return cppu::supportsService(this, rServiceName);
}

Sequence< ::rtl::OUString > SAL_CALL MyProtocolHandler::getSupportedServiceNames(  )
    throw (RuntimeException)
{
    return MyProtocolHandler_getSupportedServiceNames();
}

::rtl::OUString create_alph(sal_Unicode firstsym, sal_Unicode lastsym)
{
    ::rtl::OUString res;
    for(sal_Unicode sym = firstsym; sym <= lastsym; sym++) {
        res += ::rtl::OUString(sym);
    }
    return res;
}

::rtl::OUString create_words(::rtl::OUString alph, sal_uInt64 words_num, sal_uInt64 max_len)
{
    ::rtl::OUString res;
    std::srand(std::time(nullptr));
    for (auto i = 0; i < words_num; ++i) {
        sal_uInt64 len = std::rand() % (max_len + 1);
        if (len == 0) {
            len++;
        }
        for (auto j = 0; j < len; ++j) {
            ::rtl::OUString let(alph[std::rand() % alph.getLength()]);
            res += let;
        }
        if (i != words_num - 1) {
            res += ::rtl::OUString(" ");
        }
    }
    return res;
}

bool is_lat(sal_Unicode sym)
{
    if (sym >= 'A' && sym <= 'Z' || sym >= 'a' && sym <= 'z') {
        return true;
    }
    return false;
}

bool is_cyr(sal_Unicode sym)
{
    if (sym >= L'А' && sym <= L'я' || sym == L'ё' || sym == L'Ё') {
        return true;
    }
    return false;
}

sal_Unicode make_low(sal_Unicode sym)
{
    if (sym >= 'A' && sym <= 'Z') {
        return sym + 'a' - 'A';
    } else if (sym >= L'А' && sym <= L'Я') {
        return sym + L'а' - L'А';
    } else if (sym == L'Ё') {
        return L'ё';
    } else {
        return sym;
    }
}

std::map<sal_Unicode, long long> make_stat(::rtl::OUString text) 
{
    std::map<sal_Unicode, long long> stat;
    sal_Unicode cur_sym;
    for (auto i = 0; i < text.getLength(); ++i) {
        cur_sym = make_low(text[i]);
        if (is_cyr(cur_sym) || is_lat(cur_sym)) {
            if (stat.find(cur_sym) == stat.end()) {
                stat[cur_sym] = 1;
            } else {
                stat[cur_sym]++;
            }
        }
    }
    return stat;
}

void insert_string(Reference <com::sun::star::text::XTextTable> table, rtl::OUString name, rtl::OUString str)
{
    auto cell = table->getCellByName(name);
    Reference<com::sun::star::text::XText> cellText(cell, UNO_QUERY);
    auto cursor = cellText -> createTextCursor();
    cursor -> setString(str);
}

void SAL_CALL BaseDispatch::dispatch( const URL& aURL, const Sequence < PropertyValue >& lArgs ) throw (RuntimeException)
{
    /* It's necessary to hold this object alive, till this method finishes.
       May the outside dispatch cache (implemented by the menu/toolbar!)
       forget this instance during de-/activation of frames (focus!).

        E.g. An open db beamer in combination with the My-Dialog
        can force such strange situation :-(
     */
    Reference< XInterface > xSelfHold(static_cast< XDispatch* >(this), UNO_QUERY);

    if ( aURL.Protocol == "vnd.demo.complextoolbarcontrols.demoaddon:" )
    {
        if ( aURL.Path == "GenButtonCmd" ) {
            Reference <XComponentLoader> rComponentLoader(mxFrame, UNO_QUERY);
            if (words_num > 0 && maComboBoxText.toInt64() > 0 && cur_alph != "") {
                auto newWin = rComponentLoader->loadComponentFromURL("private:factory/swriter", "_blank", 0, lArgs);
                Reference <com::sun::star::text::XTextDocument> doc_text(newWin, UNO_QUERY);
                auto newText = doc_text -> getText();
                newText->setString(create_words(cur_alph, words_num, maComboBoxText.toInt64()));
            } else {
                ShowMessageBox(mxFrame, "Warning!", "All fields must be filled correctly!");
            }
        } 
        else if ( aURL.Path == "ComboboxCmd" ) {
            // remove the text if it's in our list
            Sequence< NamedValue > aRemoveArgs( 1 );
            aRemoveArgs[0].Name  = rtl::OUString( "Text" );
            aRemoveArgs[0].Value <<= maComboBoxText;
            SendCommand( aURL, ::rtl::OUString( "RemoveEntryText" ), aRemoveArgs, sal_True );

            // add the new text to the start of the list
            Sequence< NamedValue > aInsertArgs( 2 );
            aInsertArgs[0].Name = rtl::OUString( "Pos" );
            aInsertArgs[0].Value <<= sal_Int32( 0 );
            aInsertArgs[1].Name = rtl::OUString( "Text" );
            aInsertArgs[1].Value <<= maComboBoxText;
            SendCommand( aURL, ::rtl::OUString("InsertEntry"), aInsertArgs, sal_True );
        }
        else if ( aURL.Path == "SpinfieldCmd" ) {
            lArgs[1].Value >>= words_num;
        }
        else if ( aURL.Path == "DropdownboxCmd" ) {
            // Retrieve the text argument from the sequence property value
            ::rtl::OUString field;
            lArgs[1].Value >>= field;
            if (field == "Latin") {
                cur_alph = create_alph('a', 'z') + create_alph('A', 'Z');
            } else if (field == "Cyrillic") {
                cur_alph = create_alph(L'А', L'я') + ::rtl::OUString(L'ё') + ::rtl::OUString(L'Ё');
            } else if (field == "Mixed") {
                cur_alph = create_alph('a', 'z') + create_alph('A', 'Z');
                cur_alph += create_alph(L'А', L'я') + ::rtl::OUString(L'ё') + ::rtl::OUString(L'Ё');
            }
        }
        else if ( aURL.Path == "StatButtonCmd" ) {
            auto mxData = (mxFrame -> getController()) -> getModel();
            Reference <com::sun::star::text::XTextDocument> mxDoc(mxData, UNO_QUERY);
            auto cur_text = mxDoc->getText();
            auto cursor = cur_text -> createTextCursor();
            cursor->gotoEnd(true);
            ::rtl::OUString text = cursor->getString();
            auto stat = make_stat(text);

            Reference<com::sun::star::lang::XMultiServiceFactory> oDocMSF (mxDoc, UNO_QUERY);
            Reference <com::sun::star::text::XTextTable> table (oDocMSF->createInstance("com.sun.star.text.TextTable"),UNO_QUERY);
            table -> initialize(stat.size() + 1, 2);

            Reference <com::sun::star::text::XTextContent> xTextContent (table,UNO_QUERY);
            cur_text ->insertTextContent(cur_text->getEnd(), xTextContent, 0);
            auto cells = table->getCellNames();

            insert_string(table, cells[0], "Letter");
            insert_string(table, cells[1], "Amount");
            sal_Int64 i = 2;
            for (auto &[key, value]: stat) {
                insert_string(table, cells[i], ::rtl::OUString(key));
                ++i;
                auto cell = table->getCellByName(cells[i]);
                cell->setValue(value);
                ++i;
            }
        } 
        else if ( aURL.Path == "ColorButtonCmd" ) {
            auto mxData = (mxFrame -> getController()) -> getModel();
            Reference <com::sun::star::text::XTextDocument> mxDoc(mxData, UNO_QUERY);
            auto cursor = (mxDoc->getText()) -> createTextCursor();
            Reference <com::sun::star::beans::XPropertySet> graph(cursor, UNO_QUERY);
            ::rtl::OUString str;
            sal_Unicode sym;
            bool latword = false, cyrword = false;
            while(cursor->goRight(1, true)) {
                str = cursor->getString();
                sym = str[str.getLength()-1];
                if (is_lat(sym)) {
                    latword = true;
                } else if (is_cyr(sym)) {
                    cyrword = true;
                } else {
                    cursor->goLeft(1,true);
                    if (latword && cyrword) {
                        graph->setPropertyValue("CharBackColor", Any(0xFF0000));
                    }
                    cursor->goRight(1, false);
                    latword = false;
                    cyrword = false;
                }
            }
            if (latword && cyrword) {
                graph->setPropertyValue("CharBackColor", Any(0xFF0000));
            }
        }
    }
}

void SAL_CALL BaseDispatch::addStatusListener( const Reference< XStatusListener >& xControl, const URL& aURL ) throw (RuntimeException)
{
    if ( aURL.Protocol == "vnd.demo.complextoolbarcontrols.demoaddon:" )
    {
        if ( aURL.Path == "GenButtonCmd" )
        {
            // just enable this command
            ::com::sun::star::frame::FeatureStateEvent aEvent;
            aEvent.FeatureURL = aURL;
            aEvent.Source = (::com::sun::star::frame::XDispatch*) this;
            aEvent.IsEnabled = mbButtonEnabled;
            aEvent.Requery = sal_False;
            aEvent.State = Any();
            xControl->statusChanged( aEvent );
        }
        else if ( aURL.Path == "ComboboxCmd" )
        {
            // just enable this command
            ::com::sun::star::frame::FeatureStateEvent aEvent;
            aEvent.FeatureURL = aURL;
            aEvent.Source = (::com::sun::star::frame::XDispatch*) this;
            aEvent.IsEnabled = sal_True;
            aEvent.Requery = sal_False;
            aEvent.State = Any();
            xControl->statusChanged( aEvent );
        }
        else if ( aURL.Path == "SpinfieldCmd" )
        {
            // A spin button
            Sequence< NamedValue > aArgs( 5 );

            // send command to initialize spin button
            aArgs[0].Name = "Value";
            aArgs[0].Value <<= 1;
            aArgs[1].Name = "UpperLimit";
            aArgs[1].Value <<= INTMAX_MAX-1;
            aArgs[2].Name = "LowerLimit";
            aArgs[2].Value <<= 1;
            aArgs[3].Name = "Step";
            aArgs[3].Value <<= 1;
            aArgs[4].Name = "OutputFormat";
            aArgs[4].Value <<= rtl::OUString("%d");

            SendCommandTo( xControl, aURL, rtl::OUString( "SetValues" ), aArgs, sal_True );
        }
        else if ( aURL.Path == "DropdownboxCmd" )
        {
            // A dropdown box is normally used for a group of commands
            // where the user can select one of a defined set.
            Sequence< NamedValue > aArgs( 1 );

            // send command to set context menu content
            Sequence< rtl::OUString > aList( 3 );
            aList[0] = "Cyrillic";
            aList[1] = "Latin";
            aList[2] = "Mixed";

            aArgs[0].Name = "List";
            aArgs[0].Value <<= aList;
            SendCommandTo( xControl, aURL, rtl::OUString( "SetList" ), aArgs, sal_True );
        }

        aListenerHelper.AddListener( mxFrame, xControl, aURL.Path );
    }
}

void SAL_CALL BaseDispatch::removeStatusListener( const Reference< XStatusListener >& xControl, const URL& aURL ) throw (RuntimeException)
{
    aListenerHelper.RemoveListener( mxFrame, xControl, aURL.Path );
}

void SAL_CALL BaseDispatch::controlEvent( const ControlEvent& Event ) throw (RuntimeException)
{
    if ( Event.aURL.Protocol == "vnd.demo.complextoolbarcontrols.demoaddon:" )
    {
        if ( Event.aURL.Path == "ComboboxCmd" )
        {
            // We get notifications whenever the text inside the combobox has been changed.
            // We store the new text into a member.
            if ( Event.Event == "TextChanged" )
            {
                rtl::OUString aNewText;
                sal_Bool      bHasText( sal_False );
                for ( sal_Int32 i = 0; i < Event.aInformation.getLength(); i++ )
                {
                    if ( Event.aInformation[i].Name == "Text" )
                    {
                        bHasText = Event.aInformation[i].Value >>= aNewText;
                        break;
                    }
                }
                if ( bHasText )
                    maComboBoxText = aNewText;
            }
        }
    }
}

BaseDispatch::BaseDispatch( const Reference< XComponentContext > &rxContext,
                            const Reference< XFrame >& xFrame,
                            const ::rtl::OUString& rServiceName )
        : mxContext( rxContext )
        , mxFrame( xFrame )
        , msDocService( rServiceName )
        , mbButtonEnabled( sal_True )
{
}

BaseDispatch::~BaseDispatch()
{
    mxFrame.clear();
    mxContext.clear();
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
