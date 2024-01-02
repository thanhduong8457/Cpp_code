#include <iostream>
#include <string>
#include <vector>

using namespace std;

typedef struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(NULL) {}
    ListNode(int x) : val(x), next(NULL) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}

}ListNode_t;

//Function to insert the data at the head of the linked list
//Notice that the head of the linked list is passed by reference
//to avoid data copying(wastage of time and memory)
void insert_at_head(ListNode* &head, int data) {
    if (head == NULL) {
        head = new ListNode(data);
        return;
    }

    ListNode *temp = new ListNode(data);
    temp->next = head;
    head = temp;

    return;
}

void print_linked_list(const ListNode* head) {
    //check if the head contains some data or not
    if (head == NULL) return;

    cout << "Head is: ";

    while (head != NULL) {
        cout << head->val << "--> ";
        head = head->next;
    }

    cout << "Null" << endl;
}

class AddTwoNumbers {
public:
    AddTwoNumbers(void) {};
    ~AddTwoNumbers() {};
    ListNode* addTwoNumbers( ListNode* l1, ListNode* l2){
        ListNode *ml1 = l1;
        ListNode *ml2 = l2;
        ListNode *mListNode = NULL;
        ListNode *mListNode_return = NULL;
        ListNode *temp = NULL;

        int val1 = 0;
        int val2 = 0;

        bool remind = false;
        bool isout1 = false;
        bool isout2 = false;

        //std::cout << "add two number has been run" << std::endl;

        while (!(isout1&&isout2)) {
            temp = new ListNode(0);
            val1 = 0;
            val2 = 0;

            if (!isout1 && ml1 != NULL) {
                val1 = ml1->val;
            }
            if (!isout2 && ml2 != NULL) {
                val2 = ml2->val;
            }

            if (ml1 == NULL) {
                temp->val = val2;
            } else if (ml2 == NULL) {
                temp->val = val1;
            } else {
                temp->val = val1 + val2;
            }

            if (remind) {
                temp->val += 1;
                remind = false;
            }
            if (temp->val > 9) {
                temp->val = ((temp->val) - 10);
                remind = true;
            }

            temp->next = mListNode;
            mListNode = temp;

            if (ml1->next != NULL) {
                ml1 = ml1->next;
            }
            else {
                isout1 = true;
            }

            if (ml2->next != NULL) {
                ml2 = ml2->next;
            }
            else {
                isout2 = true;
            }
        }

        if (true == remind) {
            temp = new ListNode(1);

            temp->next = mListNode;
            mListNode = temp;
        }

        while (mListNode != NULL){
            temp = new ListNode(0);
            temp->val = mListNode->val;

            temp->next = mListNode_return;
            mListNode_return = temp;

            mListNode = mListNode->next;
        }

        return mListNode_return;
    }
};

int main(void)
{
    ListNode* head1 = NULL;

    insert_at_head(head1, 9);
    insert_at_head(head1, 9);
    insert_at_head(head1, 9);
    insert_at_head(head1, 9);
    insert_at_head(head1, 9);
    insert_at_head(head1, 9);
    insert_at_head(head1, 9);

    ListNode* head2 = NULL;

    insert_at_head(head2, 9);
    insert_at_head(head2, 9);
    insert_at_head(head2, 9);
    insert_at_head(head2, 9);

    ListNode *mListNode_sum;
    mListNode_sum = new ListNode(0);

    AddTwoNumbers *mAddTwoNumbers;
    mAddTwoNumbers = new AddTwoNumbers();

    mListNode_sum = mAddTwoNumbers->addTwoNumbers(head1, head2);

    print_linked_list(head1);
    print_linked_list(head2);
    print_linked_list(mListNode_sum);

    delete mListNode_sum;
    delete mAddTwoNumbers;
    return 0;
}
