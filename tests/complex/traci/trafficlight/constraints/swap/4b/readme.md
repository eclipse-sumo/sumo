**Startpositionen:**

Urspr. Reihenfolge: `t_0`, `t_1`, `t_2`, `t_3`

```.text
t_0 ----
        |
t_2 ----------------                
                    |               
----------------------------------------
                    |
t_1 ----------------
        |
t_3 ----


```

**Aktion:**

Es wird die Reihenfolge der oberen Fahrzeuge `t_2`, `t_0` sowie der unteren Fahrzeuge `t_3`, `t_1` getauscht. 

**Aktuelles Verhalten:**

Aktuell sind ist an der Schnittstelle aller Fahrzeuge nach den Swaps nur noch das Constraints vorhanden, 
dass  `t_2` vor `t_3` fahren darf. Die weitere Reihenfolge liegt in der Hand von SUMO und ist somit nicht mehr via swapConstraints steuerbar.

**Gewünschtes Verhalten:**

Es liegen an der Schnittstelle vollständige Constraints vor. Erwartet wird vermutlich am ehesten das Verhalten, dass 
nie grundlos weitere Fahrzeuge zur Deadlock-Verhinderung überholt werden: 

* überholt  `t_3` das Fahrzeug `t_1`, so sollte auch `t_2` das Fahrzeug `t_1` überholen (sonst Deadlock).
* überholt  `t_2` das Fahrzeug `t_0`, so sollte auch `t_1` das Fahrzeug `t_0` überholen (sonst Deadlock).

Somit gilt unabhängig von der Reihenfolge der Aufrufe die gleiche gewünschte Reigenfolge. Gerne nochmal verifizieren.


**Gewünschte Reihenfolge:**

`t_2`, `t_0`, `t_3`, `t_1`


**Herleitung - Unabhängigkeit der Call-Reihenfolge - gerne überprüfen, ob hier ein Denkfehler vorliegt :) **

Version I:
* Call I: `t_2` vor `t_0` -> `t_1` vor `t_0` (sonst Deadlock) -> `t_1`, `t_2`, `t_0`, `t_3`
* Call II: `t_3` vor `t_1` -> `t_2` & `t_0` vor `t_1` (sonst Deadlock) -> `t_2`, `t_0`, `t_3`, `t_1`

Version II:
* Call I: `t_3` vor `t_1` -> `t_2` vor `t_1` (sonst Deadlock) -> `t_0`, `t_2`, `t_3`, `t_1`
* Call II: `t_2` vor `t_0` -> keine weiteren Swaps notwendig -> `t_2`, `t_0`, `t_3`, `t_1`
