**Startpositionen:**

Urspr. Reihenfolge an `W_0`: `t_x`, `t_0`, `t_2`

Urspr. Reihenfolge an `W_1`: `t_0`, `t_1`, `t_2`

Hinweis: Fahrzeug `t_x` verwendet abweichend zu den anderen Fahrzeugen `EXIT_0` und nicht `EXIT_1`.

```.text
t_0, t_x --------
                |
t_2 ----------[W_0]--------------------------- EXIT_0               
                            |               
t_1 ----------------------[W_1]--------------- EXIT_1

```

**Aktion:**

Es wird die Reihenfolge der oberen Fahrzeuge `t_2`, `t_x` an `W_0` geändert. 

**Aktuelles Verhalten:**

Es entsteht ein Deadlock, vor `W_0`, da `t_x` auf `t_2` und `t_2` auf `t_0` wartet. 


**Gewünschtes Verhalten:**

Es existieren weiterhin für alle Reihenfolgeentscheidungen Constraints. 

* Mit dem Call  `t_2` vor `t_x` an `W_0`, muss auch `t_2` das Fahrzeug `t_0` an `W_0` überholen (sonst Deadlock an `W_0`, da `t_0` nicht mehr `t_x` passieren kann).
* Beim Call `t_2` vor `t_0` an `W_0`, muss auch `t_1` das Fahrzeug `t_0` an `W_1` überholen (sonst Deadlock an `W_1`).

Somit entsteht vermutlich folgende Call-Sequenz:
```.text
swapConstraints("t_2 vor t_x an W_0")               [User-Input]
    |-> swapConstraints("t_2 vor t_0 an W_0")       [Deadlock-Verhinderung Tiefe I]
        |-> swapConstraints("t_1 vor t_0 an W_1")   [Deadlock-Verhinderung Tiefe II]  
```

**Gewünschte Reihenfolge:**

`t_1`, `t_2`, `t_0` an `W_1`
