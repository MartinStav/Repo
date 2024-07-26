/**
 * The MainGUI class represents the main entry point of the application
 * and initializes the graphical user interface for the Election System.
 */
package index.view;

import index.controller.Controller;
import index.controller.FileErrorException;
import index.controller.UserAuthentication;
import index.controller.BackgroundProcess;
import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

import java.io.IOException;

import static java.lang.System.exit;

public class MainGUI extends Application {

    /**
     * The main controller instance for handling application logic.
     */
    public static Controller Controller = new Controller();

    /**
     * Starts the JavaFX application by loading the FXML file, initializing
     * the scene and stage, and displaying the graphical user interface.
     *
     * @param stage The primary stage for the application.
     * @throws IOException If an error occurs during loading of the FXML file.
     */
    @Override
    public void start(Stage stage) throws IOException, FileErrorException {
        FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource("/index.fxml"));
        Parent root = fxmlLoader.load();
        Scene scene = new Scene(root, 600, 600);
        stage.setTitle("Election System");
        stage.setResizable(false);
        stage.setScene(scene);
        stage.show();

        // Perform user authentication
        UserAuthentication.findAdmin();
        UserAuthentication.findUser();

        // Start background process
        BackgroundProcess backgroundProcess = new BackgroundProcess(Controller);
        Thread thread = new Thread(backgroundProcess);
        thread.start();

        // Exit application on close
        stage.setOnCloseRequest(event -> {
            exit(0);
        });
    }

    /**
     * The main method launches the JavaFX application.
     *
     * @param args Command-line arguments (not used).
     */
    public static void main(String[] args) {
        launch();
    }

}
