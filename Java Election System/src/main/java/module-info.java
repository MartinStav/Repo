module index {
    requires javafx.controls;
    requires javafx.fxml;


    exports index.controller;
    opens index.controller to javafx.fxml;
    exports index.view;
    opens index.view to javafx.fxml;
    exports index.model;
    opens index.model to javafx.fxml;
}