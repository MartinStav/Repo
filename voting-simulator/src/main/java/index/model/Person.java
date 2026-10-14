package index.model;

/**
 * The Person class represents a generic person in the system with basic attributes.
 */
public class Person {

    private String name;
    private String password;
    private int age;
    private Character role;

    /**
     * Constructs a Person object with the specified name, password, age, and role.
     *
     * @param name      The name of the person.
     * @param password  The password of the person.
     * @param age       The age of the person.
     * @param role      The role of the person.
     */
    public Person(String name, String password, int age, Character role) {
        this.name = name;
        this.password = password;
        this.age = age;
        this.role = role;
    }

    /**
     * Retrieves the name of the person.
     *
     * @return The name of the person.
     */
    public String getName() {
        return name;
    }

    /**
     * Retrieves the password of the person.
     *
     * @return The password of the person.
     */
    public String getPassword() {
        return password;
    }

    /**
     * Retrieves the age of the person.
     *
     * @return The age of the person.
     */
    public int getAge() {
        return age;
    }

    /**
     * Retrieves the role of the person.
     *
     * @return The role of the person.
     */
    public Character getRole() {
        return role;
    }

    /**
     * Returns a string representation of the person object.
     *
     * @return A string representation of the person object.
     */
    public String toString(){
        return getName() + "," + getPassword();
    }
}
