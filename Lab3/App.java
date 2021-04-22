import CNF.Clause;
import CNF.InferenceEngine;

import java.util.Set;

public class App {
    public static void main(String[] args) throws Exception {
        Clause a = new Clause("sol glass", "snöängel");
        Clause b = new Clause("snöskoter snöängel", "is badar");
        Clause c = new Clause("sol is", "promenad glass");

        InferenceEngine engine = new InferenceEngine(new Clause[] {a, b, c});
        Set<Clause> solved = engine.solve();

        for (Clause s : solved) {
            System.out.println(s);
        }
    }
}
