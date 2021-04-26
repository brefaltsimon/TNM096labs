import CNF.Clause;
import CNF.InferenceEngine;

import java.util.Set;

public class App {
    public static void main(String[] args) throws Exception {

        // FIRST PARTS

        // Bob example, correct CNF
        // Clause[] knowledgeBase = new Clause[] {
        //     new Clause("ice", "sun money"),
        //     new Clause("ice movie", "money"),
        //     new Clause("money", "movie"),
        //     new Clause("", "movie ice"),
        //     new Clause("sun money cry", ""),
        //     // New information to draw conclusisons from
        //     new Clause("movie", "")
        // };

        // Hat game (TASK B)
        // Clause[] knowledgeBase = new Clause[] {
        //     new Clause("blue", "seeRed"),
        //     new Clause("red seeRed", ""),
        //     // What the player observes
        //     new Clause("seeRed", "")
        // };

        // Trivial example of contradiction
        // A and B are true, and A -> B, B -> C
        // C is then given as false
        // Clause[] knowledgeBase = new Clause[] {
        //     // This is given
        //     new Clause("B", "A"),
        //     new Clause("C", "B"),
        //     new Clause("A", ""),
        //     new Clause("B", ""),
        //     // This is the contradiction
        //     // new Clause("C", "")
        // };

        // (-wumpus_UR V stench_UL) &
        // (-wumpus_LL V stench_UL) &
        // (-wumpus_UR V stench_LR) &
        // (-wumpus_LL V stench_LR) &
        // (wumpus_UR V wumpus_LL V -stench_UL V -stench_LR) &
        // (-wumpus_UL V stench_UR) &
        // (-wumpus_LR V stench_UR) &
        // (-wumpus_UL V stench_LL) &
        // (-wumpus_LR V stench_LL) &
        // (wumpus_UL V wumpus_LR V -stench_UR V -stench_LL) &
        // (-pit_UR V breeze_UL) &
        // (-pit_LL V breeze_UL) &
        // (-pit_UR V breeze_LR) &
        // (-pit_LL V breeze_LR) &
        // (pit_UR V pit_LL V -breeze_UL V -breeze_LR) &
        // (-pit_UL V breeze_UR) &
        // (-pit_LR V breeze_UR) &
        // (-pit_UL V breeze_LL) &
        // (-pit_LR V breeze_LL) &
        // (pit_UL V pit_LR V -breeze_UR V -breeze_LL)
        Clause[] knowledgeBase = new Clause[] {
            // Rules
            new Clause("sUL", "wUR"),
            new Clause("sUL", "wLL"),
            new Clause("sLR", "wUR"),
            new Clause("sLR", "wLL"),
            new Clause("wUR wLL", "sUL sLR"),
            new Clause("sUR", "wUL"),
            new Clause("sUR", "wLR"),
            new Clause("sLL", "wUL"),
            new Clause("sLL", "wLR"),
            new Clause("wUL wLR", "sUR sLL"),
            // new Clause("bUL", "pUR"),
            // new Clause("bUL", "pLL"),
            // new Clause("bLR", "pUR"),
            // new Clause("bLR", "pLL"),
            // new Clause("bUR", "pUR"),
            // new Clause("bUR", "pLR"),
            // new Clause("bLL", "pUL"),
            // new Clause("bLL", "pLR"),
            // Observations
            new Clause("sLR", ""),
            // new Clause("sUL", ""),
            new Clause("", "sUR"),
            new Clause("", "wUR"),
            new Clause("", "wLR")
            // new Clause("", "wUL")
        };


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
