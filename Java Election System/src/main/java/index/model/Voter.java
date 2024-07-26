package index.model;

import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;

/**
 * The Voter class represents a registered voter in the election system,
 * inheriting properties from the Person class.
 */
public class Voter extends Person implements IVoterInterface {

    private final int id;
    private static List<Voter> voters = new ArrayList<>();
    private String votedFor;

    /**
     * Constructs a Voter object with the specified name, password, age, id, and votedFor.
     *
     * @param name      The name of the voter.
     * @param password  The password of the voter.
     * @param age       The age of the voter.
     * @param id        The ID of the voter.
     * @param votedFor  The candidate the voter voted for.
     */
    public Voter(String name, String password, int age, int id, String votedFor) {
        super(name, password, age, 'U');
        this.id = id;
        this.votedFor = votedFor;
    }

    /**
     * Adds a new voter to the list of registered voters.
     *
     * @param voter The voter to be added.
     */
    public static void addVoter(Voter voter){
        voters.add(voter);
    }

    /**
     * Retrieves the list of registered voters.
     *
     * @return The list of registered voters.
     */
    public static List<Voter> getVoters(){
        return voters;
    }

    /**
     * Retrieves the ID of the voter.
     *
     * @return The ID of the voter.
     */
    public int getId(){
        return id;
    }

    /**
     * Returns a string representation of the voter object.
     *
     * @return A string representation of the voter object.
     */
    @Override
    public String toString(){
        return getName() + "," + getPassword() + "," + getAge() + "," + getRole() + "," + getId() + "," + getVotedFor();
    }

    /**
     * Retrieves the candidate the voter voted for.
     *
     * @return The candidate the voter voted for.
     */
    public String getVotedFor() {
        return votedFor;
    }

    /**
     * Sets the candidate the voter voted for and updates the authentication file.
     *
     * @param votedFor The candidate the voter voted for.
     */
    @Override
    public void setVotedFor(String votedFor) {
        this.votedFor = votedFor;
        try (PrintWriter writer = new PrintWriter("src/main/resources/Authentication.csv")) {
            writer.println(Admin.getInstance());
            for (Voter voter : voters) {
                writer.println(voter);
            }

        } catch (FileNotFoundException e) {
            System.out.println("Error writing from Authentication.csv");
        }
    }
}
