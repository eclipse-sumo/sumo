using Eclipse.Sumo.Libtraci;

internal class Program {
    static void Main(string[] args) {
        Simulation.start(new StringVector(new string[] { "sumo", "-c", "data/config.sumocfg" }));
        for (int i = 0; i < 5; i++) {
            Simulation.step();
        }
        Simulation.close();
    }
}
