package index.controller;

import index.model.Admin;
import index.model.Voter;

import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import java.util.Scanner;

/**
 * The UserAuthentication class handles user authentication and registration processes.
 */
public class UserAuthentication{

    static Random random = new Random();

    /**
     * Writes the admin details to the authentication file.
     */
    public static void writeAdmin() throws FileErrorException{
        try {
            BufferedWriter out = new BufferedWriter(new FileWriter("src/main/resources/Authentication.csv", true));
            if (Controller.getAdmin() != null) {
                out.write(Controller.getAdmin() + "\n");
            }
            out.close();
        } catch (IOException e) {
            throw new FileErrorException("Error writing to Authentication.csv, UserAuthentication.writeAdmin()");
        }
    }

    /**
     * Finds and initializes the admin from the authentication file.
     *
     * @return True if admin is found, otherwise false.
     */
    public static boolean findAdmin() throws FileErrorException {
        List<String> adminDetails = new ArrayList<>();
        try {
            Scanner scanner = new Scanner(new File("src/main/resources/Authentication.csv"));
            while (scanner.hasNextLine()) {
                adminDetails.add(scanner.nextLine());
            }
        } catch (FileNotFoundException e) {
            throw new FileErrorException("Error reading from Authentication.csv, UserAuthentication.findAdmin()");
        }

        for (String detail : adminDetails) {
            String[] info = detail.split(",");
            if (info[3].equals("A")) {
                Admin admin = Admin.getInstance(info[0], info[1], true);
                System.out.println("Admin " + admin.getName() + " with password " + admin.getPassword() + " found in file.");
                return true;
            }
        }
        return false;
    }

    /**
     * Writes the voter details to the authentication file.
     *
     * @param voter The voter object to be written to the file.
     */
    public static void writeUser(Voter voter) throws FileErrorException {
        try {
            BufferedWriter out = new BufferedWriter(new FileWriter("src/main/resources/Authentication.csv", true));
            out.write(voter.toString() + "\n");
            out.close();
        } catch (IOException e) {
            throw new FileErrorException("Error writing to Authentication.csv, UserAuthentication.writeUser()");
        }
    }

    /**
     * Finds and initializes the users from the authentication file.
     */
    public static void findUser() throws FileErrorException {
        List<String> userDetails = new ArrayList<>();
        try {
            Scanner scanner = new Scanner(new File("src/main/resources/Authentication.csv"));
            while (scanner.hasNextLine()) {
                userDetails.add(scanner.nextLine());
            }
        } catch (FileNotFoundException e) {
            throw new FileErrorException("Error writing to Authentication.csv, UserAuthentication.findUser()");
        }

        for (String detail : userDetails) {
            String[] info = detail.split(",");
            if (info[3].equals("U")) {
                Voter user;
                if (info.length < 6) {
                    user = new Voter(info[0], info[1], Integer.parseInt(info[2]), Integer.parseInt(info[4]), "");
                } else {
                    user = new Voter(info[0], info[1], Integer.parseInt(info[2]), Integer.parseInt(info[4]), info[5]);
                }
                Voter.addVoter(user);
                System.out.println("User " + user.getName() + " with password " + user.getPassword() + " found in file.");
            }
        }
    }

    /**
     * Registers a new user with a unique ID and writes their details to the authentication file.
     *
     * @param name     The name of the user.
     * @param password The password of the user.
     * @param age      The age of the user.
     * @return The registered voter object.
     */
    public static Voter registerUser(String name, String password, int age) throws FileErrorException {
        boolean isUnique = true, isUniqueName = true;
        int randomId;
        do {
            randomId = 10000 + random.nextInt(90000);
            for (Voter voter : Voter.getVoters()) {
                if (voter.getId() == randomId) {
                    isUnique = false;
                }
                if (voter.getName().equals(name)) {
                    isUniqueName = false;
                }
            }
        } while (!isUnique);
        if (isUniqueName) {
            Voter voter = new Voter(name, password, age, randomId, "");
            Voter.addVoter(voter);
            writeUser(voter);
            return voter;
        }
        return null;
    }

    /**
     * Registers a new user with a unique ID and writes their details to the authentication file.
     *
     * @param name      The name of the user.
     * @param password  The password of the user.
     * @param age       The age of the user.
     * @param votedFor  The candidate the user voted for.
     */
    public static void registerUser(String name, String password, int age, String votedFor) throws FileErrorException {
        boolean isUnique = true, isUniqueName = true;
        int randomId;
        do {
            randomId = 10000 + random.nextInt(90000);
            for (Voter voter : Voter.getVoters()) {
                if (voter.getId() == randomId) {
                    isUnique = false;
                }
                if (voter.getName().equals(name)) {
                    isUniqueName = false;
                }
            }
        } while (!isUnique);
        if (isUniqueName) {
            Voter voter = new Voter(name, password, age, randomId, votedFor);
            Voter.addVoter(voter);
            writeUser(voter);
        }
    }
}
