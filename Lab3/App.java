import CNF.Clause;
import CNF.InferenceEngine;

import java.util.Set;

public class App {
    public static void main(String[] args) throws Exception {

        // Bob example, correct CNF
        Clause[] knowledgeBase = new Clause[] {
            new Clause("ice", "sun money"),
            new Clause("ice movie", "money"),
            new Clause("money", "movie"),
            new Clause("", "movie ice"),
            new Clause("sun money cry", ""),
            // New information to draw conclusisons from
            new Clause("movie", "")
        };

        // Hat game (TASK B)
        // Clause[] knowledgeBase = new Clause[] {
        //     new Clause("blue", "seeRed"),
        //     new Clause("red seeRed", ""),
        //     // What the player observes
        //     new Clause("seeRed", "")
        // };

        InferenceEngine problem = new InferenceEngine(knowledgeBase);
        Set<Clause> solved = problem.solve();
        InferenceEngine.removeSubsumptions(solved);

        System.out.println("===============");
        for (Clause s : solved) {
            System.out.println(s);
            System.out.println("===============");
        }
    }
}
