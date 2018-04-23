/*
 * Copyright (C) 2016
 * Deutsches Zentrum fuer Luft- und Raumfahrt e.V.
 * Institut fuer Verkehrssystemtechnik
 * 
 * German Aerospace Center
 * Institute of Transportation Systems
 * 
 */
package de.dlr.ts.lisum.gui.debug;

import java.io.IOException;
import java.io.OutputStream;
import javafx.application.Platform;
import javafx.scene.control.TextArea;

/**
 *
 */
class TextAreaConsole extends OutputStream
{    
    private final TextArea txtArea;
    private final StringBuilder buffer = new StringBuilder(128);

    
    /**
     *
     * @param txtArea
     */
    public TextAreaConsole(TextArea txtArea)
    {
        this.txtArea = txtArea;
    }

    /**
     * 
     * @param b
     * @throws IOException 
     */
    @Override
    public synchronized void write(int b) throws IOException
    {
        buffer.append((char) b);
        
        if(b == 10)
        {
            String aa = buffer.toString();
            buffer.delete(0, buffer.length());
            Platform.runLater(() -> { txtArea.appendText(aa); });
        }
    }
}
