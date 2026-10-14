package index.model;

import index.controller.FileErrorException;
import index.controller.UserAuthentication;

/**
 * The Admin class represents the administrator of the election system,
 * inheriting properties from the Person class.
 */
public class Admin extends Person {

    private static Admin instance;

    /**
     * Constructs an Admin object with the specified name, password, age, and role.
     *
     * @param name      The name of the admin.
     * @param password  The password of the admin.
     * @param age       The age of the admin.
     * @param role      The role of the admin.
     */
    public Admin(String name, String password, int age, Character role) {
        super(name, password, age, role);
    }

    /**
     * Retrieves the instance of the Admin class.
     *
     * @param Name      The name of the admin.
     * @param Password  The password of the admin.
     * @param isWritten A boolean value indicating whether admin data is already written.
     * @return The instance of the Admin class.
     */
    public static Admin getInstance(String Name, String Password, boolean isWritten) throws FileErrorException {
        if (instance == null) {
            instance = new Admin(Name, Password, 0, 'A');
            if (!isWritten) {
                UserAuthentication.writeAdmin();
            }
        }
        return instance;
    }

    /**
     * Retrieves the instance of the Admin class.
     *
     * @return The instance of the Admin class.
     */
    public static Admin getInstance() {
        return instance;
    }

    /**
     * Returns a string representation of the admin object.
     *
     * @return A string representation of the admin object.
     */
    @Override
    public String toString() {
        return getName() + "," + getPassword() + "," + getAge() + "," + getRole();
    }

}
