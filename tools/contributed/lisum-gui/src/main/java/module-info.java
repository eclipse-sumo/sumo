/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

module LisumGui {
    requires lisum.core;
    requires jersey.client;
    requires jersey.core;
    requires org.apache.httpcomponents.httpclient;
    requires org.apache.httpcomponents.httpcore;
    requires commons.logging;
    requires org.apache.commons.codec;
    requires commons.io;
    requires log4j.core;
    requires log4j.api;
    requires traas;
    requires java.xml.ws;
    requires java.xml.soap;
    requires javax.jws;
    requires java.xml.bind;
    requires com.sun.xml.ws.jaxws;
    requires java.annotation;
    requires com.sun.xml.bind;
    requires com.sun.xml.txw2;
    requires com.sun.istack.runtime;
    requires com.sun.xml.ws.policy;
    requires java.activation;
    requires gmbal.api.only;
    requires management.api;
    requires org.jvnet.staxex;
    requires com.sun.xml.streambuffer;
    requires org.jvnet.mimepull;
    requires com.sun.xml.fastinfoset;
    requires ha.api;
    requires com.sun.xml.messaging.saaj;
    requires resolver;
    requires javafx.controlsEmpty;
    requires javafx.controls;
    requires javafx.graphicsEmpty;
    requires javafx.graphics;
    requires javafx.baseEmpty;
    requires javafx.base;    
    requires java.desktop;
    
    exports de.dlr.ts.lisum.gui;
}
