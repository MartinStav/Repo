package index.controller;

import index.model.NameLibrary;
import javafx.application.Platform;

import java.util.Random;
import java.util.Timer;
import java.util.TimerTask;

/**
 * The BackgroundProcess class implements the background process for generating random voters and their votes.
 */
public class BackgroundProcess implements Runnable {

    public static double martinChance = 1;
    public static double alexChance = 1;
    public static double daniChance = 1;
    Timer timer;
    static Random random;
    static NameLibrary nameLibrary;
    private static index.controller.Controller Controller;

    /**
     * Constructor for BackgroundProcess.
     *
     * @param Controller The Controller instance to access GUI elements and update the pie chart.
     */
    public BackgroundProcess(Controller Controller) {
        timer = new Timer();
        random = new Random();
        nameLibrary = new NameLibrary();
        this.Controller = Controller;
    }

    /**
     * The run method starts the background process of generating random voters and their votes.
     */
    @Override
    public void run() {
        timer.schedule(new BackgroundProcess.customerCreation(), 0, 5000);
    }

    /**
     * The customerCreation class implements the task of generating random voters.
     */
    static class customerCreation extends TimerTask {
        public void run() {
            String RandomVote = randomVoteSelector();
            if(RandomVote.equals("null")){
                return;
            }
            int randomNumber = random.nextInt(100 - 18 + 1) + 18;
            try {
                UserAuthentication.registerUser(nameLibrary.giveRandomName(), nameLibrary.giveRandomPassword(), randomNumber, RandomVote);
            } catch (FileErrorException e) {
                throw new RuntimeException(e);
            }
            Platform.runLater(() -> Controller.pieChartUpdate());
        }
    }

    /**
     * Sets the chance of a voter voting for Martin.
     *
     * @param chance The chance value (percentage) of voting for Martin.
     */
    public static void setMartinChance(double chance){
        martinChance = chance / 100;
    }

    /**
     * Sets the chance of a voter voting for Alex.
     *
     * @param chance The chance value (percentage) of voting for Alex.
     */
    public static void setAlexChance(double chance){
        alexChance = chance / 100;
    }

    /**
     * Sets the chance of a voter voting for Dani.
     *
     * @param chance The chance value (percentage) of voting for Dani.
     */
    public static void setDaniChance(double chance){
        daniChance = chance / 100;
    }

    /**
     * Selects a random vote based on the configured chances.
     *
     * @return The randomly selected vote.
     */
    private static String randomVoteSelector(){
        int randomNumberVote = random.nextInt(30) + 1;
        if (martinChance == 0 && alexChance == 0 && daniChance == 0) {
            return "null";
        }
        if (randomNumberVote <= 10) {
            if (random.nextDouble() > martinChance && martinChance != 0) {
                return randomVoteSelector();
            } else if (martinChance == 0) {
                return randomVoteSelector();
            }
            return "Martin";
        } else if (randomNumberVote <= 20) {
            if (random.nextDouble() > alexChance && alexChance != 0) {
                return randomVoteSelector();
            } else if (alexChance == 0) {
                return randomVoteSelector();
            }
            return "Alex";
        } else {
            if (random.nextDouble() > daniChance && daniChance != 0) {
                return randomVoteSelector();
            } else if (daniChance == 0) {
                return randomVoteSelector();
            }
            return "Dani";
        }
    }
}
