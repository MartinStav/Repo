# Java Election System

This project is a simple yet robust Election System built in Java, using the Maven build tool. It is designed to simulate a real-world voting system, where voters can register, login, and cast their votes. The system also includes an admin role that can manage the election process and view real-time updates of the current vote counts.

## Features

- **User Authentication**: The system includes a secure user authentication system, which is implemented in the `UserAuthentication` class. It handles both admin and user roles.

- **Voter Management**: The `Voter` class represents a registered voter in the election system. It includes methods for adding a new voter, retrieving the list of registered voters, and setting the candidate a voter voted for.

- **Background Process**: The `BackgroundProcess` class handles background tasks for the application. It is started when the application is launched and works simultaneously with the user, creating voters in the background. The admin can set the percentage of votes for each candidate for these generated voters.

- **Real-Time Updates**: The admin can see real-time updates showing the current vote counts. They can also find users through their name or ID.

- **Candidate Description**: Each candidate in the election has a description, set by the admin, providing more information about them to the voters.

- **Persistent User Data**: The users are stored in a file, so even when the program is closed and opened again, it loads users from the previous session.

- **GUI**: The application uses JavaFX for its graphical user interface. The `MainGUI` class is the main entry point of the application and initializes the GUI.

## Code Structure

The code is organized into three main packages:

- `index.model`: Contains the model classes for the application, including `Voter`, `Person`, and `Admin`.

- `index.controller`: Contains the controller classes that handle the application logic, including `Controller`, `UserAuthentication`, and `BackgroundProcess`.

- `index.view`: Contains the view classes for the application's GUI, including `MainGUI`.

## How to Run

To run the application, simply execute the `main` method in the `MainGUI` class.

## Future Improvements

- **Enhanced Security**: Implement a more secure method of storing user passwords, such as hashing.

- **Dynamic Candidate Creation**: Enhance the system to allow for the creation of multiple candidates, rather than being limited to a pre-defined set. This would allow for a more flexible and realistic simulation of an election process.