package de.dlr.ts.lisum.gui;

import de.dlr.ts.lisum.Tools;
import de.dlr.ts.lisum.Tools;
import javafx.application.Application;
import static javafx.application.Application.launch;
import javafx.stage.Stage;


/**
 * 
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class MainApp extends Application
{

    @Override
    public void init() throws Exception
    {        
        Tools.sleepi(1000); //Time showing the splash window
    }
    
    @Override
    public void start(Stage stage) throws Exception
    {
        MainProgram.getInstance().start(stage);
    }

    /**
     * 
     *
     * @param args the command line arguments
     */
    public static void main(String[] args)
    {
        launch(args);
    }
}
