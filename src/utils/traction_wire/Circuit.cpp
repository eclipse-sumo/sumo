/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    Circuit.cpp
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
#include <cfloat>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <mutex>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>

#include <microsim/MSGlobals.h>
#ifdef HAVE_EIGEN
#ifdef _MSC_VER
#pragma warning(push)
/* Disable "conditional expression is constant" warnings. */
#pragma warning(disable: 4127)
#endif
#include "Eigen/Dense"
#include "Eigen/Sparse"
#include "Eigen/Geometry"
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif

#include "Element.h"
#include "Node.h"
#include "Circuit.h"

static std::mutex circuit_lock;

Node* Circuit::addNode(std::string name) {
    if (getNode(name) != nullptr) {
        WRITE_ERROR("The node: '" + name + "' already exists.");
        return nullptr;
    }

    if (nodes->size() == 0) {
        lastId = -1;
    }
    Node* tNode = new Node(name, this->lastId);
    if (lastId == -1) {
        tNode->setGround(true);
    }
    this->lastId++;
    circuit_lock.lock();
    this->nodes->push_back(tNode);
    circuit_lock.unlock();
    return tNode;
}

void Circuit::eraseNode(Node* node) {
    circuit_lock.lock();
    this->nodes->erase(std::remove(this->nodes->begin(), this->nodes->end(), node), this->nodes->end());
    circuit_lock.unlock();
}

double Circuit::getCurrent(std::string name) {
    Element* tElement = getElement(name);
    if (tElement == nullptr) {
        return DBL_MAX;
    }
    return tElement->getCurrent();
}

double Circuit::getVoltage(std::string name) {
    Element* tElement = getElement(name);
    if (tElement == nullptr) {
        Node* node = getNode(name);
        if (node != nullptr) {
            return node->getVoltage();
        } else {
            return DBL_MAX;
        }
    } else {
        return tElement->getVoltage();
    }
}

double Circuit::getResistance(std::string name) {
    Element* tElement = getElement(name);
    if (tElement == nullptr) {
        return -1;
    }
    return tElement->getResistance();
}

Node* Circuit::getNode(std::string name) {
    for (Node* const node : *nodes) {
        if (node->getName() == name) {
            return node;
        }
    }
    return nullptr;
}

Node* Circuit::getNode(int id) {
    for (Node* const node : *nodes) {
        if (node->getId() == id) {
            return node;
        }
    }
    return nullptr;
}

Element* Circuit::getElement(std::string name) {
    for (Element* const el : *elements) {
        if (el->getName() == name) {
            return el;
        }
    }
    for (Element* const voltageSource : *voltageSources) {
        if (voltageSource->getName() == name) {
            return voltageSource;
        }
    }
    return nullptr;
}

Element* Circuit::getElement(int id) {
    for (Element* const el : *elements) {
        if (el->getId() == id) {
            return el;
        }
    }
    return getVoltageSource(id);
}

Element* Circuit::getVoltageSource(int id) {
    for (Element* const voltageSource : *voltageSources) {
        if (voltageSource->getId() == id) {
            return voltageSource;
        }
    }
    return nullptr;
}

double Circuit::getTotalPowerOfCircuitSources() {
    double power = 0;
    for (Element* const voltageSource : *voltageSources) {
        power += voltageSource->getPower();
    }
    return power;
}

double Circuit::getTotalCurrentOfCircuitSources() {
    double current = 0;
    for (Element* const voltageSource : *voltageSources) {
        current += voltageSource->getCurrent();
    }
    return current;
}

// RICE_CHECK: Locking removed?
std::string& Circuit::getCurrentsOfCircuitSource(std::string& currents) {
    //circuit_lock.lock();
    currents.clear();
    for (Element* const voltageSource : *voltageSources) {
        currents += toString(voltageSource->getCurrent(), 4) + " ";
    }
    if (!currents.empty()) {
        currents.pop_back();
    }
    //circuit_lock.unlock();
    return currents;
}

std::vector<Element*>* Circuit::getCurrentSources() {
    std::vector<Element*>* vsources = new std::vector<Element*>(0);
    for (Element* const el : *elements) {
        if (el->getType() == Element::ElementType::CURRENT_SOURCE_traction_wire) {
            //if ((*it)->getType() == Element::ElementType::CURRENT_SOURCE_traction_wire && !isnan((*it)->getPowerWanted())) {
            vsources->push_back(el);
        }
    }
    return vsources;
}

void Circuit::lock() {
    circuit_lock.lock();
}

void Circuit::unlock() {
    circuit_lock.unlock();
}

#ifdef HAVE_EIGEN
void Circuit::removeColumn(Eigen::MatrixXd& matrix, int colToRemove) {
    const int numRows = (int)matrix.rows();
    const int numCols = (int)matrix.cols() - 1;

    if (colToRemove < numCols) {
        matrix.block(0, colToRemove, numRows, numCols - colToRemove) = matrix.rightCols(numCols - colToRemove);
    }

    matrix.conservativeResize(numRows, numCols);
}

bool Circuit::solveEquationsNRmethod(double* eqn, double* vals, std::vector<int>* removable_ids) {
    // removable_ids includes nodes with voltage source already
    int numofcolumn = (int)voltageSources->size() + (int)nodes->size() - 1;
    int numofeqs = numofcolumn - (int)removable_ids->size();

    // map equations into matrix A
    Eigen::MatrixXd A = Eigen::Map < Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> >(eqn, numofeqs, numofcolumn);

    int id;
    // remove removable columns of matrix A, i.e. remove equations corresponding to nodes with two resistors connected in series
    // RICE_TODO auto for ?
    for (std::vector<int>::reverse_iterator it = removable_ids->rbegin(); it != removable_ids->rend(); ++it) {
        id = (*it >= 0 ? *it : -(*it));
        removeColumn(A, id);
    }

    // detect number of column for each node
    // in other words: detect elements of x to certain node
    // in other words: assign number of column to the proper non removable node
    int j = 0;
    Element* tElem = nullptr;
    Node* tNode = nullptr;
    for (int i = 0; i < numofcolumn; i++) {
        tNode = getNode(i);
        if (tNode != nullptr) {
            if (tNode->isRemovable()) {
                tNode->setNumMatrixCol(-1);
                continue;
            } else {
                if (j > numofeqs) {
                    WRITE_ERROR("Index of renumbered node exceeded the reduced number of equations.");
                    break;
                }
                tNode->setNumMatrixCol(j);
                j++;
                continue;
            }
        } else {
            tElem = getElement(i);
            if (tElem != nullptr) {
                if (j > numofeqs) {
                    WRITE_ERROR("Index of renumbered element exceeded the reduced number of equations.");
                    break;
                }
                continue;
            }
        }
        // tNode == nullptr && tElem == nullptr
        WRITE_ERROR("Structural error in reduced circuit matrix.");
    }

    // map 'vals' into vector b and initialize solution x
    Eigen::Map<Eigen::VectorXd> b(vals, numofeqs);
    Eigen::VectorXd x = A.colPivHouseholderQr().solve(b);

    // initialize Jacobian matrix J and vector dx
    Eigen::MatrixXd J = A;
    Eigen::VectorXd dx;
    // initialize progressively increasing maximal number of Newton-Rhapson iterations
    int max_iter_of_NR = 10;
    // number of tested values of alpha
    int attemps = 0;
    // value of scaling parameter alpha
    double alpha = 1;
    // the best (maximum) value of alpha that guarantees the existence of solution
    alphaBest = 0;
    // reason why is alpha not 1
    alphaReason = ALPHA_NOT_APPLIED;
    // vector of alphas for that no solution has been found
    std::vector<double> alpha_notSolution;
    // initialize progressively decreasing tolerance for alpha
    double alpha_res = 1e-2;

    double currentSumActual = 0.0;
    // solution x corresponding to the alphaBest
    Eigen::VectorXd x_best = x;
    bool x_best_exist = true;

    if (x.maxCoeff() > 10e6 || x.minCoeff() < -10e6) {
        WRITE_ERROR("Initial solution x used during solving DC circuit is out of bounds.\n");
    }

    // Search for the suitable scaling value alpha
    while (true) {

        ++attemps;
        int iterNR = 0;
        // run Newton-Raphson methods
        while (true) {

            // update right-hand side vector vals and Jacobian matrix J
            // node's right-hand side set to zero
            for (int i = 0; i < numofeqs - (int) voltageSources->size(); i++) {
                vals[i] = 0;
            }
            J = A;

            int i = 0;
            for (auto& node : *nodes) {
                if (node->isGround() || node->isRemovable() || node->getNumMatrixRow() == -2) {
                    continue;
                }
                if (node->getNumMatrixRow() != i) {
                    WRITE_ERROR("wrongly assigned row of matrix A during solving the circuit");
                }
                // TODO: Range-based loop
                // loop over all node's elements
                for (auto it_element = node->getElements()->begin(); it_element != node->getElements()->end(); it_element++) {
                    if ((*it_element)->getType() == Element::ElementType::CURRENT_SOURCE_traction_wire) {
                        // if the element is current source
                        if ((*it_element)->isEnabled()) {
                            double diff_voltage;
                            int PosNode_NumACol = (*it_element)->getPosNode()->getNumMatrixCol();
                            int NegNode_NumACol = (*it_element)->getNegNode()->getNumMatrixCol();
                            // compute voltage on current source
                            if (PosNode_NumACol == -1) {
                                // if the positive node is the ground => U = 0 - phi(NegNode)
                                diff_voltage = -x[NegNode_NumACol];
                            } else if (NegNode_NumACol == -1) {
                                // if the negative node is the ground => U = phi(PosNode) - 0
                                diff_voltage = x[PosNode_NumACol];
                            } else {
                                // U = phi(PosNode) - phi(NegNode)
                                diff_voltage = (x[PosNode_NumACol] - x[NegNode_NumACol]);
                            }

                            if ((*it_element)->getPosNode() == node) {
                                // the positive current (the element is consuming energy if powerWanted > 0) is flowing from the positive node (sign minus)
                                vals[i] -= alpha * (*it_element)->getPowerWanted() / diff_voltage;
                                (*it_element)->setCurrent(-alpha * (*it_element)->getPowerWanted() / diff_voltage);
                                if (PosNode_NumACol != -1) {
                                    // -1* d_b/d_phiPos = -1* d(-alpha*P/(phiPos-phiNeg) )/d_phiPos = -1* (--alpha*P/(phiPos-phiNeg)^2 )
                                    J(i, PosNode_NumACol) -= alpha * (*it_element)->getPowerWanted() / diff_voltage / diff_voltage;
                                }
                                if (NegNode_NumACol != -1) {
                                    // -1* d_b/d_phiNeg = -1* d(-alpha*P/(phiPos-phiNeg) )/d_phiNeg = -1* (---alpha*P/(phiPos-phiNeg)^2 )
                                    J(i, NegNode_NumACol) += alpha * (*it_element)->getPowerWanted() / diff_voltage / diff_voltage;
                                }
                            } else {
                                // the positive current (the element is consuming energy if powerWanted > 0) is flowing to the negative node (sign plus)
                                vals[i] += alpha * (*it_element)->getPowerWanted() / diff_voltage;
                                //Question: sign before alpha - or + during setting current?
                                //Answer: sign before alpha is minus since we assume positive powerWanted if the current element behaves as load
                                // (*it_element)->setCurrent(-alpha * (*it_element)->getPowerWanted() / diff_voltage);
                                // Note: we should never reach this part of code since the authors assumes the negataive node of current source as the ground node
                                WRITE_WARNING("The negative node of current source is not the groud.")
                                if (PosNode_NumACol != -1) {
                                    // -1* d_b/d_phiPos = -1* d(alpha*P/(phiPos-phiNeg) )/d_phiPos = -1* (-alpha*P/(phiPos-phiNeg)^2 )
                                    J(i, PosNode_NumACol) += alpha * (*it_element)->getPowerWanted() / diff_voltage / diff_voltage;
                                }
                                if (NegNode_NumACol != -1) {
                                    // -1* d_b/d_phiNeg = -1* d(alpha*P/(phiPos-phiNeg) )/d_phiNeg = -1* (--alpha*P/(phiPos-phiNeg)^2 )
                                    J(i, NegNode_NumACol) -= alpha * (*it_element)->getPowerWanted() / diff_voltage / diff_voltage;
                                }
                            }
                        }
                    }
                }
                i++;
            }


            // RICE_CHECK @20210409 This had to be merged into the master/main manually.
            // Sum of currents going through the all voltage sources
            // the sum is over all nodes, but the nonzero nodes are only those neigboring with current sources,
            // so the sum is negative sum of currents through/from current sources representing trolleybusess
            currentSumActual = 0;
            for (i = 0; i < numofeqs - (int)voltageSources->size(); i++) {
                currentSumActual -= vals[i];
            }
            // RICE_TODO @20210409 This epsilon should be specified somewhere as a constant. Or should be a parameter.
            if ((A * x - b).norm() < 1e-6) {
                //current limits
                if (currentSumActual > getCurrentLimit() && MSGlobals::gOverheadWireCurrentLimits) {
                    alphaReason = ALPHA_CURRENT_LIMITS;
                    alpha_notSolution.push_back(alpha);
                    if (x_best_exist) {
                        x = x_best;
                    }
                    break;
                }
                //voltage limits 70% - 120% of nominal voltage
                // RICE_TODO @20210409 Again, these limits should be parametrised.
                if (x.maxCoeff() > voltageSources->front()->getVoltage() * 1.2 || x.minCoeff() < voltageSources->front()->getVoltage() * 0.7) {
                    alphaReason = ALPHA_VOLTAGE_LIMITS;
                    alpha_notSolution.push_back(alpha);
                    if (x_best_exist) {
                        x = x_best;
                    }
                    break;
                }

                alphaBest = alpha;
                x_best = x;
                x_best_exist = true;
                break;
            } else if (iterNR == max_iter_of_NR) {
                alphaReason = ALPHA_NOT_CONVERGING;
                alpha_notSolution.push_back(alpha);
                if (x_best_exist) {
                    x = x_best;
                }
                break;
            }

            // Newton=Rhapson iteration
            dx = -J.colPivHouseholderQr().solve(A * x - b);
            x = x + dx;
            ++iterNR;
        }

        if (alpha_notSolution.empty()) {
            // no alpha without solution is in the alpha_notSolution, so the solving procedure is terminating
            break;
        }

        if ((alpha_notSolution.back() - alphaBest) < alpha_res) {
            max_iter_of_NR = 2 * max_iter_of_NR;
            // RICE_TODO @20210409 Why division by 10?
            // it follows Sevcik, Jakub, et al. "Solvability of the Power Flow Problem in DC Overhead Wire Circuit Modeling." Applications of Mathematics (2021): 1-19.
            // see Alg 2 (progressive decrease of optimality tolerance)
            alpha_res = alpha_res / 10;
            // RICE_TODO @20210409 This epsilon should be specified somewhere as a constant. Or should be a parameter.
            if (alpha_res < 5e-5) {
                break;
            }
            alpha = alpha_notSolution.back();
            alpha_notSolution.pop_back();
            continue;
        }

        alpha = alphaBest + 0.5 * (alpha_notSolution.back() - alphaBest);
    }

    // vals is pointer to memory and we use it now for saving solution x_best instead of right-hand side b
    for (int i = 0; i < numofeqs; i++) {
        vals[i] = x_best[i];
    }

    // RICE_TODO: Describe what is hapenning here.
    // we take x_best and alphaBest and update current values in current sources in order to be in agreement with the solution
    int i = 0;
    for (auto& node : *nodes) {
        if (node->isGround() || node->isRemovable() || node->getNumMatrixRow() == -2) {
            continue;
        }
        if (node->getNumMatrixRow() != i) {
            WRITE_ERROR("wrongly assigned row of matrix A during solving the circuit");
        }
        for (auto it_element = node->getElements()->begin(); it_element != node->getElements()->end(); it_element++) {
            if ((*it_element)->getType() == Element::ElementType::CURRENT_SOURCE_traction_wire) {
                if ((*it_element)->isEnabled()) {
                    double diff_voltage;
                    int PosNode_NumACol = (*it_element)->getPosNode()->getNumMatrixCol();
                    int NegNode_NumACol = (*it_element)->getNegNode()->getNumMatrixCol();
                    if (PosNode_NumACol == -1) {
                        diff_voltage = -x_best[NegNode_NumACol];
                    } else if (NegNode_NumACol == -1) {
                        diff_voltage = x_best[PosNode_NumACol];
                    } else {
                        diff_voltage = (x_best[PosNode_NumACol] - x_best[NegNode_NumACol]);
                    }

                    if ((*it_element)->getPosNode() == node) {
                        (*it_element)->setCurrent(-alphaBest * (*it_element)->getPowerWanted() / diff_voltage);
                    } else {
                        //Question: sign before alpha - or + during setting current?
                        //Answer: sign before alpha is minus since we assume positive powerWanted if the current element behaves as load
                        // (*it_element)->setCurrent(-alphaBest * (*it_element)->getPowerWanted() / diff_voltage);
                        // Note: we should never reach this part of code since the authors assumes the negataive node of current source as the ground node
                        WRITE_WARNING("The negative node of current source is not the groud.")
                    }
                }
            }
        }
        i++;
    }

    return true;
}
#endif

void Circuit::deployResults(double* vals, std::vector<int>* removable_ids) {
    // vals are the solution x

    int numofcolumn = (int)voltageSources->size() + (int)nodes->size() - 1;
    int numofeqs = numofcolumn - (int)removable_ids->size();

    //loop over non-removable nodes: we assign the computed voltage to the non-removables nodes
    int j = 0;
    Element* tElem = nullptr;
    Node* tNode = nullptr;
    for (int i = 0; i < numofcolumn; i++) {
        tNode = getNode(i);
        if (tNode != nullptr)
            if (tNode->isRemovable()) {
                continue;
            } else {
                if (j > numofeqs) {
                    WRITE_ERROR("Results deployment during circuit evaluation was unsuccessfull.");
                    break;
                }
                tNode->setVoltage(vals[j]);
                j++;
                continue;
            } else {
            tElem = getElement(i);
            if (tElem != nullptr) {
                if (j > numofeqs) {
                    WRITE_ERROR("Results deployment during circuit evaluation was unsuccessfull.");
                    break;
                }
                // tElem should be voltage source - the current through voltage source is computed in a loop below
                // if tElem is current source (JS thinks that no current source's id <= numofeqs), the current is already assign at the end of solveEquationsNRmethod method
                continue;
            }
        }
        WRITE_ERROR("Results deployment during circuit evaluation was unsuccessfull.");
    }

    Element* el1 = nullptr;
    Element* el2 = nullptr;
    Node* nextNONremovableNode1 = nullptr;
    Node* nextNONremovableNode2 = nullptr;
    // interpolate result of voltage to removable nodes
    for (Node* const node : *nodes) {
        if (!node->isRemovable()) {
            continue;
        }
        if (node->getElements()->size() != 2) {
            continue;
        }

        el1 = node->getElements()->front();
        el2 = node->getElements()->back();
        nextNONremovableNode1 = el1->getTheOtherNode(node);
        nextNONremovableNode2 = el2->getTheOtherNode(node);
        double x = el1->getResistance();
        double y = el2->getResistance();

        while (nextNONremovableNode1->isRemovable()) {
            el1 = nextNONremovableNode1->getAnOtherElement(el1);
            x += el1->getResistance();
            nextNONremovableNode1 = el1->getTheOtherNode(nextNONremovableNode1);
        }

        while (nextNONremovableNode2->isRemovable()) {
            el2 = nextNONremovableNode2->getAnOtherElement(el2);
            y += el2->getResistance();
            nextNONremovableNode2 = el2->getTheOtherNode(nextNONremovableNode2);
        }

        x = x / (x + y);
        y = ((1 - x) * nextNONremovableNode1->getVoltage()) + (x * nextNONremovableNode2->getVoltage());
        node->setVoltage(((1 - x)*nextNONremovableNode1->getVoltage()) + (x * nextNONremovableNode2->getVoltage()));
        node->setRemovability(false);
    }

    // Update the electric currents for voltage sources (based on Kirchhof's law: current out = current in)
    for (Element* const voltageSource : *voltageSources) {
        double currentSum = 0;
        for (Element* const el : *voltageSource->getPosNode()->getElements()) {
            // loop over all elements on PosNode excluding the actual voltage source it
            if (el != voltageSource) {
                //currentSum += el->getCurrent();
                currentSum += (voltageSource->getPosNode()->getVoltage() - el->getTheOtherNode(voltageSource->getPosNode())->getVoltage()) / el->getResistance();
                if (el->getType() == Element::ElementType::VOLTAGE_SOURCE_traction_wire) {
                    WRITE_WARNING("Cannot assign unambigous electric current value to two voltage sources connected in parallel at the same node.");
                }
            }
        }
        voltageSource->setCurrent(currentSum);
    }
}

Circuit::Circuit() {
    nodes = new std::vector<Node*>(0);
    elements = new std::vector<Element*>(0);
    voltageSources = new std::vector<Element*>(0);
    lastId = 0;
    iscleaned = true;
    circuitCurrentLimit = INFINITY;
}

Circuit::Circuit(double currentLimit) {
    nodes = new std::vector<Node*>(0);
    elements = new std::vector<Element*>(0);
    voltageSources = new std::vector<Element*>(0);
    lastId = 0;
    iscleaned = true;
    circuitCurrentLimit = currentLimit;
}

#ifdef HAVE_EIGEN
bool Circuit::_solveNRmethod() {
    double* eqn = nullptr;
    double* vals = nullptr;
    std::vector<int> removable_ids;

    detectRemovableNodes(&removable_ids);
    createEquationsNRmethod(eqn, vals, &removable_ids);
    if (!solveEquationsNRmethod(eqn, vals, &removable_ids)) {
        return false;
    }
    // vals are now the solution x of the circuit
    deployResults(vals, &removable_ids);

    delete eqn;
    delete vals;
    return true;
}

bool Circuit::solve() {
    if (!iscleaned) {
        cleanUpSP();
    }
    return this->_solveNRmethod();
}

bool Circuit::createEquationsNRmethod(double*& eqs, double*& vals, std::vector<int>* removable_ids) {
    // removable_ids does not include nodes with voltage source yet

    // number of voltage sources + nodes without the ground node
    int n = (int)(voltageSources->size() + nodes->size() - 1);
    // number of equations
    // assumption: each voltage source has different positive node and common ground node,
    //             i.e. any node excluding the ground node is connected to 0 or 1 voltage source
    int m = n - (int)(removable_ids->size() + voltageSources->size());

    // allocate and initialize zero matrix eqs and vector vals
    eqs = new double[m * n];
    vals = new double[m];

    for (int i = 0; i < m; i++) {
        vals[i] = 0;
        for (int j = 0; j < n; j++) {
            eqs[i * n + j] = 0;
        }
    }

    // loop over all nodes
    int i = 0;
    for (std::vector<Node*>::iterator it = nodes->begin(); it != nodes->end(); it++) {
        if ((*it)->isGround() || (*it)->isRemovable()) {
            // if the node is grounded or is removable set the corresponding number of row in matrix to -1 (no equation in eqs)
            (*it)->setNumMatrixRow(-1);
            continue;
        }
        assert(i < m);
        // constitute the equation corresponding to node it, add all passed voltage source elements into removable_ids
        bool noVoltageSource = createEquationNRmethod((*it), (eqs + n * i), vals[i], removable_ids);
        // if the node it has element of type "voltage source" we do not use the equation, because some value of current throw the voltage source can be always find
        if (noVoltageSource) {
            (*it)->setNumMatrixRow(i);
            i++;
        } else {
            (*it)->setNumMatrixRow(-2);
            vals[i] = 0;
            for (int j = 0; j < n; j++) {
                eqs[n * i + j] = 0;
            }
        }
    }

    // removable_ids includes nodes with voltage source already
    std::sort(removable_ids->begin(), removable_ids->end(), std::less<int>());


    for (std::vector<Element*>::iterator it = voltageSources->begin(); it != voltageSources->end(); it++) {
        assert(i < m);
        createEquation((*it), (eqs + n * i), vals[i]);
        i++;
    }

    return true;
}

bool Circuit::createEquation(Element* vsource, double* eqn, double& val) {
    if (!vsource->getPosNode()->isGround()) {
        eqn[vsource->getPosNode()->getId()] = 1;
    }
    if (!vsource->getNegNode()->isGround()) {
        eqn[vsource->getNegNode()->getId()] = -1;
    }
    if (vsource->isEnabled()) {
        val = vsource->getVoltage();
    } else {
        val = 0;
    }
    return true;
}

bool Circuit::createEquationNRmethod(Node* node, double* eqn, double& val, std::vector<int>* removable_ids) {
    // loop over all elements connected to the node
    for (std::vector<Element*>::iterator it = node->getElements()->begin(); it != node->getElements()->end(); it++) {
        double x;
        switch ((*it)->getType()) {
            case Element::ElementType::RESISTOR_traction_wire:
                if ((*it)->isEnabled()) {
                    x = (*it)->getResistance();
                    // go through all neigboring removable nodes and sum resistance of resistors in the serial branch
                    Node* nextNONremovableNode = (*it)->getTheOtherNode(node);
                    Element* nextSerialResistor = *it;
                    while (nextNONremovableNode->isRemovable()) {
                        nextSerialResistor = nextNONremovableNode->getAnOtherElement(nextSerialResistor);
                        x += nextSerialResistor->getResistance();
                        nextNONremovableNode = nextSerialResistor->getTheOtherNode(nextNONremovableNode);
                    }
                    // compute inverse value and place/add this value at proper places in eqn
                    x = 1 / x;
                    eqn[node->getId()] += x;

                    if (!nextNONremovableNode->isGround()) {
                        eqn[nextNONremovableNode->getId()] -= x;
                    }
                }
                break;
            case Element::ElementType::CURRENT_SOURCE_traction_wire:
                if ((*it)->isEnabled()) {
                    // initialize current in current source
                    if ((*it)->getPosNode() == node) {
                        x = -(*it)->getPowerWanted() / voltageSources->front()->getVoltage();
                    } else {
                        x = (*it)->getPowerWanted() / voltageSources->front()->getVoltage();
                    }
                } else {
                    x = 0;
                }
                val += x;
                break;
            case Element::ElementType::VOLTAGE_SOURCE_traction_wire:
                if ((*it)->getPosNode() == node) {
                    x = -1;
                } else {
                    x = 1;
                }
                eqn[(*it)->getId()] += x;
                // equations with voltage source can be ignored, because some value of current throw the voltage source can be always find
                removable_ids->push_back((*it)->getId());
                return false;
                break;
            case Element::ElementType::ERROR_traction_wire:
                return false;
                break;
        }
    }
    return true;
}
#endif

/**
 * Select removable nodes, i.e. nodes that are NOT the ground of the circuit
 * and that have exactly two resistor elements connected. Ids of those
 * removable nodes are added into the internal vector `removable_ids`.
 */
void Circuit::detectRemovableNodes(std::vector<int>* removable_ids) {
    // loop over all nodes in the circuit
    for (std::vector<Node*>::iterator it = nodes->begin(); it != nodes->end(); it++) {
        // if the node is connected to two elements and is not the ground
        if ((*it)->getElements()->size() == 2 && !(*it)->isGround()) {
            // set such node defaultly as removable. But check if the two elements are both resistors
            (*it)->setRemovability(true);
            for (std::vector<Element*>::iterator it2 = (*it)->getElements()->begin(); it2 != (*it)->getElements()->end(); it2++) {
                if ((*it2)->getType() != Element::ElementType::RESISTOR_traction_wire) {
                    (*it)->setRemovability(false);
                    break;
                }
            }
            if ((*it)->isRemovable()) {
                //if the node is removeable add pointer into the vector of removeblas nodes
                removable_ids->push_back((*it)->getId());
            }
        } else {
            (*it)->setRemovability(false);
        }
    }
    // sort the vector of removable ids
    std::sort(removable_ids->begin(), removable_ids->end(), std::less<int>());
    return;
}

Element* Circuit::addElement(std::string name, double value, Node* pNode, Node* nNode, Element::ElementType et) {
    // RICE_CHECK: This seems to be a bit of work in progress, is it final?
    // if ((et == Element::ElementType::RESISTOR_traction_wire && value <= 0) || et == Element::ElementType::ERROR_traction_wire) {
    if (et == Element::ElementType::RESISTOR_traction_wire && value <= 1e-6) {
        //due to numeric problems
        // RICE_TODO @20210409 This epsilon should be specified somewhere as a constant. Or should be a parameter.
        if (value > -1e-6) {
            value = 1e-6;
            WRITE_WARNING("Trying to add resistor element into the overhead wire circuit with resistance < 1e-6. ")
        } else {
            WRITE_ERROR("Trying to add resistor element into the overhead wire circuit with resistance < 0. ")
            return nullptr;
        }
    }

    Element* e = getElement(name);

    if (e != nullptr) {
        //WRITE_ERROR("The element: '" + name + "' already exists.");
        std::cout << "The element: '" + name + "' already exists.";
        return nullptr;
    }

    e = new Element(name, et, value);
    if (e->getType() == Element::ElementType::VOLTAGE_SOURCE_traction_wire) {
        e->setId(lastId);
        lastId++;
        circuit_lock.lock();
        this->voltageSources->push_back(e);
        circuit_lock.unlock();
    } else {
        circuit_lock.lock();
        this->elements->push_back(e);
        circuit_lock.unlock();
    }

    e->setPosNode(pNode);
    e->setNegNode(nNode);

    pNode->addElement(e);
    nNode->addElement(e);
    return e;
}

void Circuit::eraseElement(Element* element) {
    element->getPosNode()->eraseElement(element);
    element->getNegNode()->eraseElement(element);
    circuit_lock.lock();
    this->elements->erase(std::remove(this->elements->begin(), this->elements->end(), element), this->elements->end());
    circuit_lock.unlock();
}

void Circuit::replaceAndDeleteNode(Node* unusedNode, Node* newNode) {
    //replace element node if it is unusedNode
    for (auto& voltageSource : *voltageSources) {
        if (voltageSource->getNegNode() == unusedNode) {
            voltageSource->setNegNode(newNode);
            newNode->eraseElement(voltageSource);
            newNode->addElement(voltageSource);
        }
        if (voltageSource->getPosNode() == unusedNode) {
            voltageSource->setPosNode(newNode);
            newNode->eraseElement(voltageSource);
            newNode->addElement(voltageSource);
        }
    }
    for (auto& element : *elements) {
        if (element->getNegNode() == unusedNode) {
            element->setNegNode(newNode);
            newNode->eraseElement(element);
            newNode->addElement(element);
        }
        if (element->getPosNode() == unusedNode) {
            element->setPosNode(newNode);
            newNode->eraseElement(element);
            newNode->addElement(element);
        }
    }

    //erase unusedNode from nodes vector
    this->eraseNode(unusedNode);

    //modify id of other elements and nodes
    int modLastId = this->getLastId() - 1;
    if (unusedNode->getId() != modLastId) {
        Node* node_last = this->getNode(modLastId);
        if (node_last != nullptr) {
            node_last->setId(unusedNode->getId());
        } else {
            Element* elem_last = this->getVoltageSource(modLastId);
            if (elem_last != nullptr) {
                elem_last->setId(unusedNode->getId());
            } else {
                WRITE_ERROR("The element or node with the last Id was not found in the circuit!");
            }
        }
    }

    this->descreaseLastId();
    delete unusedNode;
}

void Circuit::cleanUpSP() {
    for (std::vector<Element*>::iterator it = elements->begin(); it != elements->end(); it++) {
        if ((*it)->getType() != Element::ElementType::RESISTOR_traction_wire) {
            (*it)->setEnabled(true);
        }
    }

    for (std::vector<Element*>::iterator it = voltageSources->begin(); it != voltageSources->end(); it++) {
        (*it)->setEnabled(true);
    }
    this->iscleaned = true;
}

bool Circuit::checkCircuit(std::string substationId) {
    // check empty nodes
    for (std::vector<Node*>::iterator it = nodes->begin(); it != nodes->end(); it++) {
        if ((*it)->getNumOfElements() < 2) {
            //cout << "WARNING: Node [" << (*it)->getName() << "] is connected to less than two elements, please enter other elements.\n";
            if ((*it)->getNumOfElements() < 1) {
                return false;
            }
        }
    }
    // check voltage sources
    for (std::vector<Element*>::iterator it = voltageSources->begin(); it != voltageSources->end(); it++) {
        if ((*it)->getPosNode() == nullptr || (*it)->getNegNode() == nullptr) {
            //cout << "ERROR: Voltage Source [" << (*it)->getName() << "] is connected to less than two nodes, please enter the other end.\n";
            WRITE_ERROR("Circuit Voltage Source '" + (*it)->getName() + "' is connected to less than two nodes, please adjust the definition of the section (with substation '" + substationId + "').");
            return false;
        }
    }
    // check other elements
    for (std::vector<Element*>::iterator it = elements->begin(); it != elements->end(); it++) {
        if ((*it)->getPosNode() == nullptr || (*it)->getNegNode() == nullptr) {
            //cout << "ERROR: Element [" << (*it)->getName() << "] is connected to less than two nodes, please enter the other end.\n";
            WRITE_ERROR("Circuit Element '" + (*it)->getName() + "' is connected to less than two nodes, please adjust the definition of the section (with substation '" + substationId + "').");
            return false;
        }
    }

    // check connectivity
    int num = (int)nodes->size() + getNumVoltageSources() - 1;
    bool* nodesVisited = new bool[num];
    for (int i = 0; i < num; i++) {
        nodesVisited[i] = false;
    }
    // TODO: Probably unused
    // int id = -1;
    if (!getNode(-1)->isGround()) {
        //cout << "ERROR: Node id -1 is not the ground \n";
        WRITE_ERROR("Circuit Node with id '-1' is not the grounded, please adjust the definition of the section (with substation '" + substationId + "').");
    }
    std::vector<Node*>* queue = new std::vector<Node*>(0);
    Node* node = nullptr;
    Node* neigboringNode = nullptr;
    //start with (voltageSources->front()->getPosNode())
    nodesVisited[voltageSources->front()->getId()] = 1;
    node = voltageSources->front()->getPosNode();
    queue->push_back(node);

    while (!queue->empty()) {
        node = queue->back();
        queue->pop_back();
        if (!nodesVisited[node->getId()]) {
            nodesVisited[node->getId()] = true;
            for (auto it = node->getElements()->begin(); it != node->getElements()->end(); it++) {
                neigboringNode = (*it)->getTheOtherNode(node);
                if (!neigboringNode->isGround()) {
                    queue->push_back(neigboringNode);
                } else if ((*it)->getType() == Element::ElementType::VOLTAGE_SOURCE_traction_wire) {
                    /// there used to be == 1 which was probably a typo ... check!
                    nodesVisited[(*it)->getId()] = 1;
                } else if ((*it)->getType() == Element::ElementType::RESISTOR_traction_wire) {
                    //cout << "ERROR: The resistor type connects the ground \n";
                    WRITE_ERROR("A Circuit Resistor Element connects the ground, please adjust the definition of the section (with substation '" + substationId + "').");
                }
            }
        }
    }

    for (int i = 0; i < num; i++) {
        if (nodesVisited[i] == 0) {
            //cout << "ERROR: Node or voltage source with id " << (i) << " has been not visited during checking of the circuit => Disconnectivity of the circuit. \n";
            WRITE_WARNING("Circuit Node or Voltage Source with internal id '" + toString(i) + "' has been not visited during checking of the circuit. The circuit is disconnected, please adjust the definition of the section (with substation '" + substationId + "').");
        }
    }

    return true;
}

int Circuit::getNumVoltageSources() {
    return (int) voltageSources->size();
}
