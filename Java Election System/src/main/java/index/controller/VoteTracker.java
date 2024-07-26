package index.controller;

import index.model.Voter;

/**
 * The VoteTracker class tracks and counts the votes cast by voters.
 */
public class VoteTracker {
    private static int[] tracker = new int[3];

    /**
     * Retrieves the vote tracker array.
     *
     * @return The vote tracker array.
     */
    public static int[] getTracker() {
        return tracker;
    }

    /**
     * Counts the votes cast by voters and determines the leading candidate.
     *
     * @return The name of the leading candidate.
     */
    public static String voteCount() {
        // Initialize tracker
        for (int i = 0; i < 3; i++){
            tracker[i] = 0;
        }

        // Count votes
        for(Voter voter : Voter.getVoters()){
            if (voter.getVotedFor().equals("Martin")){
                tracker[0]++;
            } else if (voter.getVotedFor().equals("Alex")){
                tracker[1]++;
            } else{
                tracker[2]++;
            }
        }

        // Determine leading candidate
        int maxIndex = 0;
        for (int i = 1; i < tracker.length; i++) {
            if (tracker[i] > tracker[maxIndex]) {
                maxIndex = i;
            }
        }

        // Return the leading candidate
        if (maxIndex == 0) {
            return "Martin";
        } else if (maxIndex == 1) {
            return "Alex";
        } else {
            return "Other";
        }
    }
}
