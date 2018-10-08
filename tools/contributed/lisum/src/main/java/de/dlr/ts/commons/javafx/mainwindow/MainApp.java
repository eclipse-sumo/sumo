/*
 * Copyright (C) 2016
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.commons.javafx.mainwindow;

import javafx.application.Application;
import javafx.stage.Stage;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class MainApp extends Application
{
    private MainWindow mw;
    
    
    @Override
    public void start(Stage primaryStage) throws Exception
    {
        primaryStage.setTitle("Main Window 1.0");
        
        mw = new MainWindow(primaryStage, 1200, 900);
        mw.show();
    }
    
    /**
     * The main() method is ignored in correctly deployed JavaFX application. 
     *
     * @param args the command line arguments
     */
    public static void main(String[] args)
    {
        launch(args);
    }

    public MainWindow getMw()
    {
        return mw;
    }
}
