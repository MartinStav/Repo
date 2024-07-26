package index.controller;

import index.model.Admin;
import index.model.Person;
import index.model.Voter;
import javafx.beans.binding.Bindings;
import javafx.fxml.FXML;
import javafx.scene.chart.PieChart;
import javafx.scene.control.*;
import javafx.scene.image.ImageView;
import javafx.scene.layout.Pane;
import javafx.scene.text.Text;

import java.security.SecureRandom;
import java.util.Objects;

/**
 * The Controller class handles the functionality and logic of the GUI elements.
 */
public class Controller {

    public Observer observer = new Observer();
    public Pane userSignedIn;
    public Pane SignedIn;
    public Text notFound;
    public Text underAge;
    public Text userExists;
    public Text adminNotFound;
    public Text votedFor;
    public ImageView winningMartinImg;
    public ImageView winningAlexImg;
    public ImageView winningDaniImg;
    public Text winningNoone;
    public Text winningMartin;
    public Text winningAlex;
    public Text winningDani;
    public ImageView winningMartinImg1;
    public ImageView winningAlexImg1;
    public ImageView winningDaniImg1;
    public Text winningNoone1;
    public Text winningMartin1;
    public Text winningAlex1;
    public Text winningDani1;
    public TextField findUserNameId;
    public PieChart pieChart;
    public Text foundUserNameId;
    public TextArea martinDescriptionChange;
    public Slider martinPercentageChange;
    public TextArea alexDescriptionChange;
    public Slider alexPercentageChange;
    public TextArea daniDescriptionChange;
    public Slider daniPercentageChange;
    public TextArea alexDescriptionShow;
    public TextArea martinDescriptionShow;
    public TextArea daniDescriptionShow;

    @FXML
    private Pane AdminLogin;
    @FXML
    private Pane StartView;
    @FXML
    private Pane LoginRegisterView;

    private static SecureRandom random = new SecureRandom();
    private String generatedPassword;
    private static final String CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    @FXML
    private TextField AdminLoginName;
    @FXML
    private PasswordField AdminLoginPassword;
    @FXML
    private TextField UserRegisterName;
    @FXML
    private PasswordField UserRegisterPassword;
    @FXML
    private TextField UserRegisterAge;
    @FXML
    private TextField UserLoginName;
    @FXML
    private PasswordField UserLoginPassword;

    private Person loggedUser;
    String selected = "";

    static PieChart.Data slice1 = new PieChart.Data("Martin", 0);
    static PieChart.Data slice2 = new PieChart.Data("Alex", 0);
    static PieChart.Data slice3 = new PieChart.Data("Dani", 0);

    /**
     * Switches the view to the admin login screen.
     */
    @FXML
    private void AdminView() {
        StartView.setVisible(false);
        AdminLogin.setVisible(true);
        generatePassword();
        createPieChart();
    }

    /**
     * Switches the view to the user login/register screen.
     */
    @FXML
    private void UserView() {
        StartView.setVisible(false);
        LoginRegisterView.setVisible(true);
    }

    /**
     * Switches the view back to the start view.
     */
    public void BackToStartView() {
        AdminLogin.setVisible(false);
        LoginRegisterView.setVisible(false);
        userSignedIn.setVisible(false);
        SignedIn.setVisible(false);
        StartView.setVisible(true);
        loggedUser = null;
    }

    /**
     * Generates a random password for admin login.
     */
    public void generatePassword() {
        StringBuilder password = new StringBuilder(8);
        for (int i = 0; i < 8; i++) {
            password.append(CHARACTERS.charAt(random.nextInt(CHARACTERS.length())));
        }
        generatedPassword = password.toString();
        System.out.println(generatedPassword);
    }

    /**
     * Handles admin login functionality.
     */
    public void AdminLogin() throws FileErrorException {
        if (!UserAuthentication.findAdmin()) {
            if (Objects.equals(generatedPassword, AdminLoginPassword.getText())) {
                Admin admin = Admin.getInstance(AdminLoginName.getText(), AdminLoginPassword.getText(), false);
                System.out.println("Admin " + admin.getName() + " logged in.");
                AdminLogin.setVisible(false);
                SignedIn.setVisible(true);
                adminNotFound.setVisible(false);
                observer.update();

                showWinner();
            } else {
                adminNotFound.setVisible(true);
            }
        } else if (AdminLoginName.getText().equals(Admin.getInstance().getName()) && AdminLoginPassword.getText().equals(Admin.getInstance().getPassword())) {
            System.out.println("Admin " + Admin.getInstance().getName() + " logged in.");
            AdminLogin.setVisible(false);
            SignedIn.setVisible(true);
            adminNotFound.setVisible(false);
            observer.update();
            showWinner();
        } else {
            adminNotFound.setVisible(true);
        }
    }

    /**
     * Retrieves the admin details.
     *
     * @return Admin details.
     */
    public static String getAdmin() {
        if (Admin.getInstance() != null) {
            return Admin.getInstance().toString();
        }
        return null;
    }

    /**
     * Handles user registration functionality.
     */
    public void UserRegister() throws FileErrorException {
        if (Integer.parseInt(UserRegisterAge.getText()) >= 18) {
            if(loggedUser instanceof Voter){
                if ((loggedUser = UserAuthentication.registerUser(UserRegisterName.getText(), UserRegisterPassword.getText(), Integer.parseInt(UserRegisterAge.getText()))) != null) {
                    AdminLogin.setVisible(false);
                    userSignedIn.setVisible(true);
                    votedFor.setText(!Objects.equals(((Voter) loggedUser).getVotedFor(), "") ? ((Voter) loggedUser).getVotedFor() : "No vote recorded yet.");
                    showWinner();
                } else {
                    userExists.setVisible(true);
                    underAge.setVisible(false);
                }
            }
        } else {
            underAge.setVisible(true);
            userExists.setVisible(false);
        }
    }

    /**
     * Handles user login functionality.
     */
    public void UserLogin() {
        for (Voter voter : Voter.getVoters()) {
            if (UserLoginName.getText().equals(voter.getName()) && UserLoginPassword.getText().equals(voter.getPassword())) {
                loggedUser = voter;
                System.out.println("User " + voter.getName() + " logged in.");
                LoginRegisterView.setVisible(false);
                userSignedIn.setVisible(true);
                notFound.setVisible(false);
                votedFor.setText(!Objects.equals(voter.getVotedFor(), "") ? voter.getVotedFor() : "No vote recorded yet.");
                showWinner();
            } else {
                notFound.setVisible(true);
            }
        }
    }

    /**
     * Handles the user voting functionality.
     */
    public void voteCandidate() {
        if (!Objects.equals(selected, "")) {
            if(loggedUser instanceof Voter) {
                ((Voter) loggedUser).setVotedFor(selected);
                votedFor.setText(((Voter) loggedUser).getVotedFor());
            }
        }
    }

    /**
     * Displays the winner based on the vote count.
     */
    public void showWinner() {
        String mostVotes = VoteTracker.voteCount();
        if (mostVotes.equals("Martin")) {
            winningNoone.setVisible(false);
            winningAlex.setVisible(false);
            winningAlexImg.setVisible(false);
            winningDani.setVisible(false);
            winningDaniImg.setVisible(false);
            winningMartin.setVisible(true);
            winningMartinImg.setVisible(true);
        } else if (mostVotes.equals("Alex")) {
            winningNoone.setVisible(false);
            winningAlex.setVisible(true);
            winningAlexImg.setVisible(true);
            winningDani.setVisible(false);
            winningDaniImg.setVisible(false);
            winningMartin.setVisible(false);
            winningMartinImg.setVisible(false);
        } else {
            winningNoone.setVisible(false);
            winningAlex.setVisible(false);
            winningAlexImg.setVisible(false);
            winningDani.setVisible(true);
            winningDaniImg.setVisible(true);
            winningMartin.setVisible(false);
            winningMartinImg.setVisible(false);
        }
        showWinnerAdmin(mostVotes);
    }

    /**
     * Displays the winner for the admin view.
     *
     * @param mostVotes Most voted candidate.
     */
    public void showWinnerAdmin(String mostVotes) {
        if (mostVotes.equals("Martin")) {
            winningNoone1.setVisible(false);
            winningAlex1.setVisible(false);
            winningAlexImg1.setVisible(false);
            winningDani1.setVisible(false);
            winningDaniImg1.setVisible(false);
            winningMartin1.setVisible(true);
            winningMartinImg1.setVisible(true);
        } else if (mostVotes.equals("Alex")) {
            winningNoone1.setVisible(false);
            winningAlex1.setVisible(true);
            winningAlexImg1.setVisible(true);
            winningDani1.setVisible(false);
            winningDaniImg1.setVisible(false);
            winningMartin1.setVisible(false);
            winningMartinImg1.setVisible(false);
        } else {
            winningNoone1.setVisible(false);
            winningAlex1.setVisible(false);
            winningAlexImg1.setVisible(false);
            winningDani1.setVisible(true);
            winningDaniImg1.setVisible(true);
            winningMartin1.setVisible(false);
            winningMartinImg1.setVisible(false);
        }
    }

    /**
     * Handles candidate selection for voting.
     */
    public void Select1() {
        selected = "Martin";
    }

    /**
     * Handles candidate selection for voting.
     */
    public void Select2() {
        selected = "Alex";
    }

    /**
     * Handles candidate selection for voting.
     */
    public void Select3() {
        selected = "Dani";
    }

    /**
     * Finds a user by name or id.
     */
    public void FindByNameId() throws NumberFormatException {
        String input = findUserNameId.getText();
        try {
            for (Voter voter : Voter.getVoters()) {
                if (voter.getId() == Integer.parseInt(input)) {
                    foundUserNameId.setText(voter.getId() + ", name: " + voter.getName() + ", age: " + voter.getAge() + ", password: " + voter.getPassword() + ", voted for: " + voter.getVotedFor());
                    break;
                } else {
                    foundUserNameId.setText("Did not found user.");
                }
            }
        } catch (NumberFormatException e) {
            for (Voter voter : Voter.getVoters()) {
                if (voter.getName().equals(input)) {
                    foundUserNameId.setText(voter.getId() + ", name: " + voter.getName() + ", age: " + voter.getAge() + ", password: " + voter.getPassword() + ", voted for: " + voter.getVotedFor());
                    break;
                } else {
                    foundUserNameId.setText("Did not found user.");
                }
            }
        }
    }

    /**
     * Creates the pie chart.
     */
    public void createPieChart() {
        if (!pieChart.getData().isEmpty()) {
            pieChart.getData().clear();
        }
        pieChart.getData().addAll(slice1, slice2, slice3);
    }

    /**
     * Updates the pie chart with the latest vote count.
     */
    public void pieChartUpdate() {
        VoteTracker.voteCount();
        slice1.setPieValue(VoteTracker.getTracker()[0]);
        slice2.setPieValue(VoteTracker.getTracker()[1]);
        slice3.setPieValue(VoteTracker.getTracker()[2]);
        slice1.nameProperty().bind(Bindings.concat("Martin: ", VoteTracker.getTracker()[0]));
        slice2.nameProperty().bind(Bindings.concat("Alex: ", VoteTracker.getTracker()[1]));
        slice3.nameProperty().bind(Bindings.concat("Dani: ", VoteTracker.getTracker()[2]));
    }

    /**
     * Submits the changes made by the admin.
     */
    public void SubmitChange() {
        martinDescriptionShow.setText(martinDescriptionChange.getText());
        alexDescriptionShow.setText(alexDescriptionChange.getText());
        daniDescriptionShow.setText(daniDescriptionChange.getText());
        BackgroundProcess.setMartinChance((int) martinPercentageChange.getValue());
        BackgroundProcess.setAlexChance((int) alexPercentageChange.getValue());
        BackgroundProcess.setDaniChance((int) daniPercentageChange.getValue());
    }
}