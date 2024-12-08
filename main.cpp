// coba upload ke github versi 2.

#include <iostream>
#include <string>

using namespace std;

struct LawEnforcementData {
    int id;
    string firmName;
    string suspectName;
    string jurisdiction;
    string caseStatus;
};

struct elmList {
    LawEnforcementData data;
    elmList* prev;
    elmList* next;
};

typedef elmList* address;

struct CircularDoublyLinkedList {
    address head = nullptr;
    address cursor = nullptr;
};

// Undo-Redo Action Structure
struct UndoRedoAction {
    string type; // ActionType changed to string
    LawEnforcementData actionData;
    int actionId;
    UndoRedoAction* next = nullptr;
};

struct UndoRedoStack {
    UndoRedoAction* top = nullptr;
};

struct UndoRedoManager {
    UndoRedoStack undoStack;
    UndoRedoStack redoStack;
    int nextId = 1;

    int generateUniqueId() {
        return nextId++;
    }

    void pushUndoAction(UndoRedoAction action) {
        UndoRedoAction* newAction = new UndoRedoAction(action);
        newAction->next = undoStack.top;
        undoStack.top = newAction;
        clearRedoStack();
    }

    UndoRedoAction popUndo() {
        if (undoStack.top == nullptr) {
            cout << "No undo actions available" << endl;
            return UndoRedoAction(); // Return an empty action
        }
        UndoRedoAction* temp = undoStack.top;
        UndoRedoAction action = *temp;
        undoStack.top = temp->next;
        delete temp;
        return action;
    }

    UndoRedoAction popRedo() {
        if (redoStack.top == nullptr) {
            cout << "No redo actions available" << endl;
            return UndoRedoAction(); // Return an empty action
        }
        UndoRedoAction* temp = redoStack.top;
        UndoRedoAction action = *temp;
        redoStack.top = temp->next;
        delete temp;
        return action;
    }

    void pushRedoAction(UndoRedoAction action) {
        UndoRedoAction* newAction = new UndoRedoAction(action);
        newAction->next = redoStack.top;
        redoStack.top = newAction;
    }

    void clearRedoStack() {
        while (redoStack.top != nullptr) {
            UndoRedoAction* temp = redoStack.top;
            redoStack.top = temp->next;
            delete temp;
        }
    }

    bool canUndo() {
        return undoStack.top != nullptr;
    }

    bool canRedo() {
        return redoStack.top != nullptr;
    }
};

struct CircularLawEnforcementList {
    CircularDoublyLinkedList list;
    UndoRedoManager undoRedoManager;

    address createNewNode() {
        address p = new elmList();
        p->prev = nullptr;
        p->next = nullptr;
        return p;
    }

    // Insertion methods
    void insertAtCursor(LawEnforcementData data) {
        address p = createNewNode();
        p->data = data;

        if (list.head == nullptr) {
            list.head = p;
            list.cursor = p;
            p->next = p;
            p->prev = p;
        } else {
            p->next = list.cursor->next;
            p->prev = list.cursor;
            list.cursor->next->prev = p;
            list.cursor->next = p;
            list.cursor = p;
        }

        UndoRedoAction action = { "INSERT_AT", data, data.id };
        undoRedoManager.pushUndoAction(action);
    }

    void insertBeforeCursor(LawEnforcementData data) {
        address p = createNewNode();
        p->data = data;

        if (list.head == nullptr) {
            list.head = p;
            list.cursor = p;
            p->next = p;
            p->prev = p;
        } else {
            p->prev = list.cursor->prev;
            p->next = list.cursor;
            list.cursor->prev->next = p;
            list.cursor->prev = p;

            if (list.cursor == list.head) {
                list.head = p;
            }
            list.cursor = p;
        }

        UndoRedoAction action = { "INSERT_BEFORE", data, data.id };
        undoRedoManager.pushUndoAction(action);
    }

    void insertAfterCursor(LawEnforcementData data) {
        address p = createNewNode();
        p->data = data;

        if (list.head == nullptr) {
            list.head = p;
            list.cursor = p;
            p->next = p;
            p->prev = p;
        } else {
            p->next = list.cursor->next;
            p->prev = list.cursor;
            list.cursor->next->prev = p;
            list.cursor->next = p;
            list.cursor = p;
        }

        UndoRedoAction action = { "INSERT_AFTER", data, data.id };
        undoRedoManager.pushUndoAction(action);
    }

    // Deletion methods
    LawEnforcementData deleteAtCursor() {
        if (list.head == nullptr) {
            cout << "List is empty" << endl;
            return LawEnforcementData();
        }

        address nodeToDelete = list.cursor;
        LawEnforcementData deletedData = nodeToDelete->data;

        if (list.head->next == list.head) {
            delete list.head;
            list.head = list.cursor = nullptr;
        } else {
            nodeToDelete->prev->next = nodeToDelete->next;
            nodeToDelete->next->prev = nodeToDelete->prev;

            if (nodeToDelete == list.head) {
                list.head = nodeToDelete->next;
            }
            list.cursor = nodeToDelete->next;
            delete nodeToDelete;
        }

        UndoRedoAction action = { "DELETE_AT", deletedData, deletedData.id };
        undoRedoManager.pushUndoAction(action);

        return deletedData;
    }

    LawEnforcementData deleteBeforeCursor() {
        if (list.head == nullptr) {
            cout << "List is empty" << endl;
            return LawEnforcementData();
        }

        address nodeToDelete = list.cursor->prev;
        LawEnforcementData deletedData = nodeToDelete->data;

        if (list.head->next == list.head) {
            delete list.head;
            list.head = list.cursor = nullptr;
        } else {
            nodeToDelete->prev->next = list.cursor;
            list.cursor->prev = nodeToDelete->prev;

            if (nodeToDelete == list.head) {
                list.head = list.cursor;
            }
            delete nodeToDelete;
        }

        UndoRedoAction action = { "DELETE_BEFORE", deletedData, deletedData.id };
        undoRedoManager.pushUndoAction(action);

        return deletedData;
    }

    LawEnforcementData deleteAfterCursor() {
        if (list.head == nullptr) {
            cout << "List is empty" << endl;
            return LawEnforcementData();
        }

        address nodeToDelete = list.cursor->next;
        LawEnforcementData deletedData = nodeToDelete->data;

        if (list.head->next == list.head) {
            delete list.head;
            list.head = list.cursor = nullptr;
        } else {
            nodeToDelete->next->prev = list.cursor;
            list.cursor->next = nodeToDelete->next;

            if (nodeToDelete == list.head) {
                list.head = nodeToDelete->next;
            }
            delete nodeToDelete;
        }

        UndoRedoAction action = { "DELETE_AFTER", deletedData, deletedData.id };
        undoRedoManager.pushUndoAction(action);

        return deletedData;
    }

    // Undo and Redo methods (updated to handle new actions)
    void performUndo() {
        if (!undoRedoManager.canUndo()) {
            cout << "No actions to undo." << endl;
            return;
        }

        UndoRedoAction action = undoRedoManager.popUndo();
        if (action.type == "INSERT_AT" || action.type == "INSERT_BEFORE" || action.type == "INSERT_AFTER") {
            deleteAtCursor();
        } else if (action.type == "DELETE_AT" || action.type == "DELETE_BEFORE" || action.type == "DELETE_AFTER") {
            insertAtCursor(action.actionData);
        }
        undoRedoManager.pushRedoAction(action);
    }

    void performRedo() {
        if (!undoRedoManager.canRedo()) {
            cout << "No actions to redo." << endl;
            return;
        }

        UndoRedoAction action = undoRedoManager.popRedo();
        if (action.type == "INSERT_AT" || action.type == "INSERT_BEFORE" || action.type == "INSERT_AFTER") {
            insertAtCursor(action.actionData);
        } else if (action.type == "DELETE_AT" || action.type == "DELETE_BEFORE" || action.type == "DELETE_AFTER") {
            deleteAtCursor();
        }
        undoRedoManager.pushUndoAction(action);
    }

    int generateId() {
        return undoRedoManager.generateUniqueId();
    }

    void displayList() {
        if (list.head == nullptr) {
            cout << "List is empty." << endl;
            return;
        }

        address current = list.head;
        do {
            if (current == list.cursor) {
                cout << "[";
            }
            cout << "ID: " << current->data.id
                 << ", Firm: " << current->data.firmName
                 << ", Suspect: " << current->data.suspectName
                 << ", Jurisdiction: " << current->data.jurisdiction
                 << ", Status: " << current->data.caseStatus;
            if (current == list.cursor) {
                cout << "] <cursor>";
            }
            cout << " -> ";
            current = current->next;
        } while (current != list.head);
        cout << endl;
    }
};

int main() {
    CircularLawEnforcementList lawList;

    // Create initial data
    LawEnforcementData fbi = {
        lawList.generateId(),
        "FBI",
        "John Doe Cartel Leader",
        "National",
        "Active Investigation"
    };
    lawList.insertAtCursor(fbi);

    LawEnforcementData lapd = {
        lawList.generateId(),
        "LAPD",
        "Street Gang Suspect",
        "Los Angeles",
        "Ongoing Surveillance"
    };
    lawList.insertAtCursor(lapd);

    LawEnforcementData nypd = {
        lawList.generateId(),
        "NYPD",
        "Organized Crime Figure",
        "New York City",
        "High-Priority Case"
    };
    lawList.insertAtCursor(nypd);

    cout << "Initial List:" << endl;
    lawList.displayList();

    // Demonstrate new insertion methods
    LawEnforcementData beforeInsert = {
        lawList.generateId(),
        "Homeland Security",
        "Border Trafficking Suspect",
        "Border Zone",
        "Initial Detection"
    };
    cout << "\nInserting before cursor:" << endl;
    lawList.insertBeforeCursor(beforeInsert);
    lawList.displayList();

    LawEnforcementData afterInsert = {
        lawList.generateId(),
        "DEA",
        "Drug Trafficking Investigation",
        "Interstate",
        "Active Pursuit"
    };
    cout << "\nInserting after cursor:" << endl;
    lawList.insertAfterCursor(afterInsert);
    lawList.displayList();

    // Demonstrate new deletion methods
    cout << "\nDeleting before cursor:" << endl;
    lawList.deleteBeforeCursor();
    lawList.displayList();

    cout << "\nDeleting after cursor:" << endl;
    lawList.deleteAfterCursor();
    lawList.displayList();

    // Demonstrate undo and redo with new methods
    cout << "\nUndoing last action:" << endl;
    lawList.performUndo();
    lawList.displayList();

    cout << "\nRedoing last action:" << endl;
    lawList.performRedo();
    lawList.displayList();

    return 0;
}
