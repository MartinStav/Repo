package index.controller;

import index.view.MainGUI;

/**
 * The Observer class updates the MainGUI controller to trigger a pie chart update.
 */
public class Observer {

    /**
     * Updates the MainGUI controller to trigger a pie chart update.
     */
    void update() {
        MainGUI.Controller.pieChartUpdate();
    }
}
