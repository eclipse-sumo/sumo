/*
 * Copyright (C) 2016
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.lisum.gui.misc;

import de.dlr.ts.lisum.gui.SystemProperties;
import javafx.scene.Scene;
import javafx.scene.effect.Bloom;
import javafx.scene.image.ImageView;
import javafx.scene.layout.StackPane;
import javafx.stage.Stage;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class Splash
{
    private final Scene scene;
    private final Stage stage;
    
    private final ImageView imageView = new ImageView(SystemProperties.getInstance().getMainIcon());
    private final StackPane root = new StackPane(imageView);
    
    /**
     * 
     */
    public Splash()
    {
        Bloom bloom = new Bloom();
        bloom.setThreshold(0.7);
        imageView.setEffect(bloom);
        
        imageView.setStyle(  
                            "-fx-background-color: rgba(255, 255, 255, 1.0);" 
                        +   "-fx-effect: dropshadow(gaussian, gray, 50, 0, 0, 0);" 
                        +   "-fx-background-insets: 10;"
        );

        scene = new Scene(root, 300, 350);
        //scene.setFill(Color.TRANSPARENT);
        stage = new Stage();
        stage.setScene(scene);
        //stage.initStyle(StageStyle.TRANSPARENT);
        stage.centerOnScreen();
        
    }
    
}
