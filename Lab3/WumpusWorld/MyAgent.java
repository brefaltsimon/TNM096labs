import CNF.*;

public class MyAgent implements Agent {
    private World w;

    public MyAgent(World world) {
        w = world;
    }

    

    // Ask your solver to do an action

    public void doAction() {
        // Location of the player
        int cX = w.getPlayerX();
        int cY = w.getPlayerY();

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

        // Test the environment
        if (w.hasBreeze(cX, cY)) {
            System.out.println("I am in a Breeze");
        }
        if (w.hasStench(cX, cY)) {
            System.out.println("I am in a Stench");
        }
        if (w.hasPit(cX, cY)) {
            System.out.println("I am in a Pit");
        }

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
