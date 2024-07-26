package index.model;

import java.util.ArrayList;
import java.util.Random;

/**
 * The NameLibrary class provides a library of random names and surnames
 * to be used for generating user data.
 */
public class NameLibrary {
    private ArrayList<String> names = new ArrayList<>();
    private ArrayList<String> surnames = new ArrayList<>();

    /**
     * Constructs a NameLibrary object and initializes lists of names and surnames.
     */
    public NameLibrary() {
        names.add("John");
        names.add("Emma");
        names.add("Michael");
        names.add("Sophia");
        names.add("William");
        names.add("Olivia");
        names.add("James");
        names.add("Ava");
        names.add("Robert");
        names.add("Mia");
        names.add("David");
        names.add("Isabella");
        names.add("Joseph");
        names.add("Charlotte");
        names.add("Daniel");
        names.add("Amelia");
        names.add("Matthew");
        names.add("Harper");
        names.add("Andrew");
        names.add("Evelyn");
        names.add("Benjamin");
        names.add("Abigail");
        names.add("Emily");
        names.add("Alexander");
        names.add("Sofia");
        names.add("Henry");

        surnames.add("Smith");
        surnames.add("Johnson");
        surnames.add("Williams");
        surnames.add("Jones");
        surnames.add("Brown");
        surnames.add("Davis");
        surnames.add("Miller");
        surnames.add("Wilson");
        surnames.add("Moore");
        surnames.add("Taylor");
        surnames.add("Anderson");
        surnames.add("Thomas");
        surnames.add("Jackson");
        surnames.add("White");
        surnames.add("Harris");
        surnames.add("Martin");
        surnames.add("Thompson");
        surnames.add("Garcia");
        surnames.add("Martinez");
        surnames.add("Robinson");
        surnames.add("Clark");
        surnames.add("Rodriguez");
        surnames.add("Lewis");
        surnames.add("Lee");
        surnames.add("Walker");
        surnames.add("Hall");
    }

    /**
     * Retrieves a random name from the list of names.
     *
     * @return A random name.
     */
    public String giveRandomName() {
        Random random = new Random();
        return names.get(random.nextInt(names.size()));
    }
    /**
     * Retrieves a random surname from the list of surnames.
     *
     * @return A random surname.
     */
    public String giveRandomPassword() {
        Random random = new Random();
        return surnames.get(random.nextInt(surnames.size()));
    }
}
