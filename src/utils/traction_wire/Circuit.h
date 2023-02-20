/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    Circuit.h
/// @author  Jakub Sevcik (RICE)
/// @author  Jan Prikryl (RICE)
/// @date    2019-12-15
///
/// @note    based on console-based C++ DC circuits simulator,
///          https://github.com/rka97/Circuits-Solver by
///          Ahmad Khaled, Ahmad Essam, Omnia Zakaria, Mary Nader
///          and available under MIT license, see https://github.com/rka97/Circuits-Solver/blob/master/LICENSE
///
// Representation of electric circuit of overhead wires
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>
#ifdef HAVE_EIGEN
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4464 5031)
#endif
// avoid warnings in clang
#ifdef __clang__
#pragma clang system_header
#endif
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/Sparse>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif

#include "Element.h"

// ===========================================================================
// class declarations
// ===========================================================================
class Node;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * All interactions will be through this class, the user will know nothing about the other classes,
 * and will interact only through the names of the elements/nodes.
 */
class Circuit {

private:

    std::vector<Node*>* nodes;
    std::vector<Element*>* elements;
    std::vector<Element*>* voltageSources;

    int lastId;
    bool iscleaned;

    /// @brief The electric current limit of the voltage sources.
    double circuitCurrentLimit;

    /**
    * @brief Best alpha scaling value.
    *
    * This parameter is used to scale down the power demands of current sources (vehicles
    * that draw power from the circuit) so that a solution of the system can be found.
    * Note: the system is nonlinear (quadratic), hence in some cases (typically too high
    * power demands) a solution cannot be found. In that moment we decrease all power
    * requirements by `alpha` and try to solve again, until we find alpha that ensures
    * stable solution. This is then reported as alphaBest.
    */
    double alphaBest;
public:
    /**
     * @brief Flag of alpha scaling parameter
     *
     * returns ALPHA_NOT_APPLIED => alpha should be 1
     * returns ALPHA_CURRENT_LIMITS => alpha is lower than one due to electric current limits of the substation
     * returns ALPHA_VOLTAGE_LIMITS => alpha is not one due to inability of network to transfer requested power due to overhead wire resistance
     * returns ALPHA_NOT_CONVERGING => number of allowed iterations exceeded
     */
    enum alphaFlag {
        /// @brief The scaling alpha is not applied (is one)
        ALPHA_NOT_APPLIED = 0,
        /// @brief The scaling alpha is applied (is not one) due to current limits
        ALPHA_CURRENT_LIMITS,
        /// @brief The scaling alpha is applied (is not one] due to voltage limits
        ALPHA_VOLTAGE_LIMITS,
        /// @brief The Newton-Rhapson method has reached maximum iterations and no solution of circuit has been found with actual value of alpha
        ALPHA_NOT_CONVERGING
    };
private:
    alphaFlag alphaReason;

public:
    Node* getNode(std::string name);
    Element* getElement(std::string name);
    Node* getNode(int id);
    Element* getVoltageSource(int id);
    std::vector<Element*>* getCurrentSources();

    /// @brief The sum of voltage source powers in the circuit
    double getTotalPowerOfCircuitSources();
    /// @brief The sum of voltage source currents in the circuit
    double getTotalCurrentOfCircuitSources();
    /// @brief List of currents of voltage sources as a string
    std::string& getCurrentsOfCircuitSource(std::string& currents);

    void lock();
    void unlock();

    /// @brief return alphaBest variable, the best alpha scaling value
    double getAlphaBest() {
        return alphaBest;
    };

    /// @brief return the reason why `alpha` scaling value has been used
    alphaFlag getAlphaReason() {
        return alphaReason;
    };

private:

    Element* getElement(int id);
    /*
    *    detects removable nodes = sets node variable "isremovable" to true if node is removable and adds id of such node to "removable_ids" vector
    *    node is denoted as removable if it is connected just to 2 elements and both of them are resistor
    *    the reason is that in such case there are two serial resistor and we can only sum their resistance value
    *
    *    "removable_ids" vector is sort from the least to the greatest
    */
    void detectRemovableNodes(std::vector<int>* removable_ids);

    void deployResults(double* vals, std::vector<int>* removable_ids);

#ifdef HAVE_EIGEN
    /*
    *    creates all of the equations that represent the circuit
    *    in the form Ax = B(1/x) where A and B are matricies
    *    @param eqn : A
    *    @param vals : B
    */
    bool createEquationsNRmethod(double*& eqs, double*& vals, std::vector<int>* removable_ids);

    /*
    *    creates the nodal equation of the node 'node' GV = I
    *    in the form Ax = B(1/x) where A is a matrix with one row
    *    @param node : the node to be analyzed
    *    @param eqn : A
    *    @param val : B
    */
    bool createEquationNRmethod(Node* node, double* eqn, double& val, std::vector<int>* removable_ids);

    /**
     * @brief Create the equation of the voltage source.
     * Create the equation V2 - V1 = E of the voltage source in the form Ax = B,
     * where A is a matrix with one row, B a value
     * @param[in] vsource The voltage source
     * @param[in] eqn : A
     * @param[in] val : B
     * @return ???
    */
    bool createEquation(Element* vsource, double* eqn, double& val);

    /*
     *    removes the "colToRemove"-th column from matrix "matrix"
     */
    void removeColumn(Eigen::MatrixXd& matrix, const int colToRemove);

    /*
     * solves the system of nonlinear equations Ax = B(1/x)
     * @param eqn : A
     * @param vals : B
     */
    bool solveEquationsNRmethod(double* eqn, double* vals, std::vector<int>*);

    bool _solveNRmethod();

#endif
public:

    // a Constructor, same functionality as "init" functions
    Circuit();
    // RICE_CHECK: Is this a traction substation current limit, global for all substations?
    /// @brief Constructor with user-specified current limit parameter.
    Circuit(double currentLimit);

    // adds an element with name "name", type "type" and value "value" to positive node "pNode" and negative node "nNode""
    Element* addElement(std::string name, double value, Node* pNode, Node* nNode, Element::ElementType et);

    void eraseElement(Element* element);

    // adds a node with name "name"
    Node* addNode(std::string name);

    // erases a node with name "name"
    void eraseNode(Node* node);

    // gets current through element "name"
    double getCurrent(std::string name);

    // gets voltage across element or node "name"
    double getVoltage(std::string name);

    // gets the resistance of an element.
    double getResistance(std::string name);

    // gets the number of voltage sources in the circuit.
    int getNumVoltageSources();

    // checks if the circuit's connections are correct.
    bool checkCircuit(std::string substationId = "");

#ifdef HAVE_EIGEN
    // solves the circuit and deploys the results
    bool solve();
#endif

    // cleans up after superposition.
    void cleanUpSP();

    //replaces unusedNode with newNode everywhere in the circuit, modifies the ids of other nodes and elements, descreases the id by one and deletes unusedNode
    void replaceAndDeleteNode(Node* unusedNode, Node* newNode);

    // returns lastId
    int getLastId() {
        return lastId;
    };

    // decreases lastId by one
    void descreaseLastId() {
        lastId--;
    };

    /// RICE_CHECK: Is this identical to the current limit of a traction substation?
    /// @brief Set the electric current limit of this circuit.
    void setCurrentLimit(double myCurrentLimit) {
        circuitCurrentLimit = myCurrentLimit;
    };

    /// @ brief Get the electric current limit of this circuit.
    double getCurrentLimit() {
        return circuitCurrentLimit;
    };
};
