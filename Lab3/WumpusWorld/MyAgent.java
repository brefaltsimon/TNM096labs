import CNF.*;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;

public class MyAgent implements Agent {
    private World w;
    private HashMap<String, Set<Clause>> map;

    public MyAgent(World world) {
        w = world;
        map = new HashMap<String, Set<Clause>>();
        getTile(0, 0).add(new Clause("", "p"));
        getTile(0, 0).add(new Clause("", "w"));
    }

    private String coordinatesAt(int x, int y) {
        return x + ", " + y;
    }

    private Set<Clause> getTile(int x, int y) {
        Set<Clause> tile = map.get(coordinatesAt(x, y));
        if (tile == null) {
            tile = new HashSet<>();
            map.put(coordinatesAt(x, y), tile);
        }
        return tile;
    }

    private ArrayList<Clause> getNeighborhoodKB(int LLx, int LLy) {
        String LL = Integer.toString(LLx) + Integer.toString(LLy);
        String LR = Integer.toString(LLx + 1) + Integer.toString(LLy);
        String UL = Integer.toString(LLx) + Integer.toString(LLy + 1);
        String UR = Integer.toString(LLx + 1) + Integer.toString(LLy + 1) ;

        return new ArrayList<>(Arrays.asList(new Clause[] {
                // Rules
                new Clause("s" + UL, "w" + UR),
                new Clause("s" + UL, "w" + LL),
                new Clause("s" + LR, "w" + UR),
                new Clause("s" + LR, "w" + LL),
                new Clause("w" + UR + " w" + LL, "s" + UL + " s" + LR),
                new Clause("s" + UR, "w" + UL),
                new Clause("s" + UR, "w" + LR),
                new Clause("s" + LL, "w" + UL),
                new Clause("s" + LL, "w" + LR),
                new Clause("w" + UL + " w" + LR, "s" + UR + " s" + LL),
                new Clause("b" + UL, "p" + UR),
                new Clause("b" + UL, "p" + LL),
                new Clause("b" + LR, "p" + UR),
                new Clause("b" + LR, "p" + LL),
                new Clause("p" + UR + " p" + LL, "b" + UL + " b" + LR),
                new Clause("b" + UR, "p" + UR),
                new Clause("b" + UR, "p" + LR),
                new Clause("b" + LL, "p" + UL),
                new Clause("b" + LL, "p" + LR),
                new Clause("p" + UL + " p" + LR, "b" + UR + " b" + LL)
            }
        ));
    }

    private Set<Clause> solve2by2Area(int LLx, int LLy) {
        ArrayList<Clause> neighborhood = new ArrayList<>();
        // LL
        int x = LLx;
        int y = LLy;
        for (Clause c : getTile(x, y)){
            neighborhood.add(c.getDecorated(x, y));
        }
        // LR
        x = LLx + 1;
        y = LLy;
        for (Clause c : getTile(x, y)) {
            neighborhood.add(c.getDecorated(x, y));
        }
        // UL
        x = LLx;
        y = LLy + 1;
        for (Clause c : getTile(x, y)) {
            neighborhood.add(c.getDecorated(x, y));
        }
        // UR
        x = LLx + 1;
        y = LLy + 1;
        for (Clause c : getTile(x, y)) {
            neighborhood.add(c.getDecorated(x, y));
        }

        neighborhood.addAll(getNeighborhoodKB(LLx, LLy));
        InferenceEngine problem = new InferenceEngine(neighborhood);


        return problem.solve();
    }

    private void solve3by3Area(int x, int y) {
        ArrayList<Clause> localKB = new ArrayList<>();

        // Upper left corner
        localKB.addAll(solve2by2Area(x-1, y));
        // Upper right corner
        localKB.addAll(solve2by2Area(x, y));
        // Lower left corner
        localKB.addAll(solve2by2Area(x-1, y-1));
        // Lower right corner
        localKB.addAll(solve2by2Area(x, y-1));

        System.out.println("3x3 NEIGHBORHOOD INFERENCE");
        System.out.println("======================");
        for (Clause c : new InferenceEngine(localKB).solve()) {
            System.out.println(c);
            System.out.println("======================");
        }
    }

    // Ask your solver to do an action

    public void doAction() {
        // Location of the player
        int cX = w.getPlayerX();
        int cY = w.getPlayerY();
        Set<Clause> currentTile = getTile(cX, cY);

        // Basic action:
        // Grab Gold if we can.
        if (w.hasGlitter(cX, cY)) {
            w.doAction(World.A_GRAB);
            return;
        }

        // Basic action:
        // We are in a pit. Climb up.
        if (w.isInPit()) {
            w.doAction(World.A_CLIMB);
            return;
        }

        // Check if the current position exists in the map
        if (currentTile == null) {
            currentTile = new HashSet<>();
        }

        // Test the environment
        if (w.hasBreeze(cX, cY)) {
            System.out.println("I am in a Breeze");
            currentTile.add(new Clause("b", ""));
        }
        if (w.hasStench(cX, cY)) {
            System.out.println("I am in a Stench");
            currentTile.add(new Clause("s", ""));
        }
        if (w.hasPit(cX, cY)) {
            System.out.println("I am in a Pit");
            currentTile.add(new Clause("p", ""));
            getTile(cX + 1, cY).add(new Clause("b", ""));
            getTile(cX - 1, cY).add(new Clause("b", ""));
            getTile(cX, cY + 1).add(new Clause("b", ""));
            getTile(cX, cY - 1).add(new Clause("b", ""));
        } else {
            currentTile.add(new Clause("", "p"));
        }

        currentTile.add(new Clause("", "w"));
        solve3by3Area(cX, cY);

        // for (String tile : map.keySet()) {
        //     System.out.println(tile + " has: " + map.get(tile).toString());
        // }

        // Random move actions
        int rnd = (int) (Math.random() * 3);
        if (rnd == 0) {
            w.doAction(World.A_MOVE_RIGHT);
            return;
        }
        if (rnd >= 1) {
            w.doAction(World.A_MOVE_UP);
            return;
        }
    }
}

// ACTIONS AVAILABLE
// --------------------------------
// w.doAction(World.A_MOVE_RIGHT);
// w.doAction(World.A_MOVE_LEFT);
// w.doAction(World.A_MOVE_UP);
// w.doAction(World.A_MOVE_DOWN);

// w.doAction(World.A_SHOOT_UP);
// w.doAction(World.A_SHOOT_DOWN);
// w.doAction(World.A_SHOOT_LEFT);
// w.doAction(World.A_SHOOT_RIGHT);

// w.doAction(World.A_GRAB);
// w.doAction(World.A_CLIMB)

// SENSING ACTIONS (return true/false)
// ------------------------------------
// w.hasGlitter(cX,cY)
// w.hasBreeze(cX,cY)
// w.hasStench(cX, cY)
// w.hasPit(cX, cY)
// w.hasWumpus(cX, cY)
// w.isInPit()
// w.wumpusAlive()
// w.hasArrow()
