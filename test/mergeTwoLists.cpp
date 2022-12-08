#include <iostream>
#include <string>
#include <vector>
#include <set>

using namespace std;

//Definition for singly-linked list.
struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

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

class Solution {
public:
    ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
        ListNode *ListNode_return = NULL;
        ListNode *mlist1 = list1;
        ListNode *mlist2 = list2;
        vector<int> list_merger;
        vector<int>::iterator it;

        while (mlist1 != NULL)  {
            list_merger.push_back(mlist1->val);
            mlist1 = mlist1->next;
        }

        while (mlist2 != NULL)  {
            list_merger.push_back(mlist2->val);
            mlist2 = mlist2->next;
        }

        int temp = 0;
        for (int i = 0; i < list_merger.size(); i++) {
            for (int j = i + 1; j < list_merger.size(); j++) {
                if (list_merger[i] < list_merger[j]) {
                    temp = list_merger[i];
                    list_merger[i] = list_merger[j];
                    list_merger[j] = temp;
                }
            }
        }

        ListNode *temp2;
        for (it = list_merger.begin(); it != list_merger.end(); it++) {
            temp2 = new ListNode(*it);
            temp2->next = ListNode_return;
            ListNode_return = temp2;
        }

        return ListNode_return;
    }
};

int main(void)
{
    ListNode* head1 = NULL;

    insert_at_head(head1, 1);
    insert_at_head(head1, 3);
    insert_at_head(head1, 5);
    insert_at_head(head1, 7);
    insert_at_head(head1, 9);

    ListNode* head2 = NULL;

    insert_at_head(head2, 2);
    insert_at_head(head2, 2);
    insert_at_head(head2, 4);
    insert_at_head(head2, 4);
    insert_at_head(head2, 6);
    insert_at_head(head2, 8);
    insert_at_head(head2, 8);

    ListNode *mListNode_sum;
    mListNode_sum = new ListNode(0);

    Solution *mSolution;
    mSolution = new Solution();

    mListNode_sum = mSolution->mergeTwoLists(head1, head2);

    print_linked_list(head1);
    print_linked_list(head2);
    print_linked_list(mListNode_sum);

    delete head1;
    delete head2;
    delete mListNode_sum;
    delete mSolution;
    return 0;
}
