#include "invoice_chain.hpp"

using namespace std;

Invoice_ChainNode::Invoice_ChainNode():invoice(nullptr), mem_next(nullptr), pro_next(nullptr),
                                    inv_num_next(nullptr), pro_prev(nullptr), mem_prev(nullptr) {}

Invoice_ChainNode::Invoice_ChainNode(Session_Invoice *toAdd):invoice(toAdd), mem_next(nullptr), pro_next(nullptr),
                                    inv_num_next(nullptr), pro_prev(nullptr), mem_prev(nullptr) {}

Invoice_ChainNode::~Invoice_ChainNode()
{
    delete invoice;
    invoice = nullptr;
    mem_next = nullptr;
    pro_next = nullptr;
    inv_num_next = nullptr;
    pro_prev = nullptr;
    mem_prev = nullptr;
//    delete pro_next;
//    delete mem_next;
}

Invoice_Chain::Invoice_Chain(): mem_head(nullptr), pro_head(nullptr), inv_num_head(nullptr) {}

Invoice_Chain::~Invoice_Chain()
{
    Invoice_ChainNode *del;
    pro_head = nullptr;
    mem_head = nullptr;
    while(inv_num_head) {
        del = inv_num_head;
        del->pro_next = nullptr;
        del->mem_next = nullptr;
        del->inv_num_next = nullptr;
        del->pro_prev = nullptr;
        del->mem_prev = nullptr;
        inv_num_head = inv_num_head->inv_num_next;
        delete del;
    }
}

int Invoice_Chain::populate(const string& file_in)
{
    //open the indicated file or return
    ifstream in_file(file_in);
    if(!in_file.is_open()) return 0;

    //temp variables for input
    string  servNumIn = {0}, proNumIn = {0}, memNumIn = {0}, commIn = {0};
    tm *subTimeIn = new tm, *servDateIn = new tm;
    string subYr, subMo, subDay, subHr, subMin, subSec, servYr, servMo, servDay;
    int count = 0;  //temp variable to return invoices input
    invoice_count = 0;
    *subTimeIn = {0}, *servDateIn = {0};

    //loop over input until EOF
    while(getline(in_file, servNumIn, ','))
    {
        //get temp values from file
        getline(in_file, proNumIn, ',');
        getline(in_file, memNumIn, ',');
        getline(in_file, subYr, ',');
        subTimeIn->tm_year = stoi(subYr);
        getline(in_file, subMo, ',');
        subTimeIn->tm_year = stoi(subMo);
        getline(in_file, subDay, ',');
        subTimeIn->tm_year = stoi(subDay);
        getline(in_file, subHr, ',');
        subTimeIn->tm_year = stoi(subHr);
        getline(in_file, subMin, ',');
        subTimeIn->tm_year = stoi(subMin);
        getline(in_file, subSec, ',');
        subTimeIn->tm_year = stoi(subSec);
        getline(in_file, servYr, ',');
        subTimeIn->tm_year = stoi(servYr);
        getline(in_file, servMo, ',');
        subTimeIn->tm_year = stoi(servMo);
        getline(in_file, servDay, ',');
        subTimeIn->tm_year = stoi(servDay);
        getline(in_file, commIn);

        //create person to add
        Session_Invoice *toAdd = new Session_Invoice(servNumIn, proNumIn, memNumIn, subTimeIn, servDateIn, commIn);
        toAdd->setInvNum(++invoice_count);

        //add the vehicle to the table & increment count
        add_invoice(toAdd);
        count++;
    }

    in_file.close(); //close file

    return count; //return the number read in
}

bool Invoice_Chain::add_invoice(Session_Invoice *toAdd)
{
    Invoice_ChainNode *newinvoice = new Invoice_ChainNode(toAdd);
    Invoice_ChainNode *curr = pro_head, *prev = nullptr;

//this first bit is good for both since if one empty the other empty
    if (!pro_head && !mem_head && !inv_num_head) {
        pro_head = newinvoice;
        mem_head = newinvoice;
        inv_num_head = newinvoice;
    }
    else {
//first run through to place in providers chain
        while (toAdd->getProNum() > curr->invoice->getProNum() && curr->pro_next) {
            prev = curr;
            curr = curr->pro_next;
        }
        if (!curr->pro_next) { curr->pro_next = newinvoice;}
        else {
            if(prev) prev->pro_next = newinvoice;
            newinvoice->pro_next = curr;
        }
//repeat the process for members chain
        curr = mem_head;
        prev = nullptr;
        while (toAdd->getMemNum() > curr->invoice->getMemNum() && curr->mem_next) {
            prev = curr;
            curr = curr->mem_next;
        }
        if (!curr->mem_next) { curr->mem_next = newinvoice;}
        else {
            if(prev) prev->mem_next = newinvoice;
            newinvoice->mem_next = curr;
        }
//lastly repeat it again for inv_num
        curr = inv_num_head;
        prev = nullptr;
        while (toAdd->getInvNum() > curr->invoice->getInvNum() && curr->inv_num_next) {
            prev = curr;
            curr = curr->inv_num_next;
        }
        if (!curr->inv_num_next) { curr->inv_num_next = newinvoice;}
        else {
            if(prev) prev->inv_num_next = newinvoice;
            newinvoice->inv_num_next = curr;
        }
    }
    return true;
}

Session_Invoice * Invoice_Chain::find_invoice(int inv_num) {
    if(!inv_num_head) return nullptr;
    Invoice_ChainNode *curr = inv_num_head;
    Session_Invoice *result = nullptr;

    while (curr && !result) {
        if(curr->invoice->getInvNum() == inv_num) result = curr->invoice;
        curr = curr->inv_num_next;
    }
    return result;
}

bool Invoice_Chain::edit_invoice(int inv_num)
{
    if(!inv_num_head) { return false; }
    Session_Invoice *temp = find_invoice(inv_num);
    if(!temp) {
        cout << "\n\tCould not locate record." << endl;
        return false;
    }
    return temp->edit();
}

bool Invoice_Chain::remove_invoice(int inv_num)
{
    Invoice_ChainNode *curr = inv_num_head, *prev = nullptr, *del = nullptr;

    if (!inv_num_head) { return false; }
    else {
        while (curr) {
            if (curr->invoice->getInvNum() == inv_num) {
                del = curr;
                if (curr == inv_num_head) { inv_num_head = curr->inv_num_next; }
                else { prev->inv_num_next = curr->inv_num_next; }

                if (curr == pro_head) { pro_head = curr->pro_next; }
                else {
                    curr->pro_prev->pro_next = curr->pro_next;
                    if(curr->pro_next) { curr->pro_next->pro_prev = curr->pro_prev; }
                }

                if (curr == mem_head) { mem_head = curr->mem_next; }
                else {
                    curr->mem_prev->mem_next = curr->mem_next;
                    if(curr->mem_next) { curr->mem_next->mem_prev = curr->mem_prev; }
                }
                delete del;
            }
            prev = curr;
            curr = curr->inv_num_next;
        }
    }
    return true;
}

void Invoice_Chain::display_all()
{
    if(!inv_num_head) return;
    Invoice_ChainNode *curr = inv_num_head;
    while (curr) {
        curr->invoice->display();
        cout << endl;
    }
}

bool Invoice_Chain::write_out(string file_out)
{
    ofstream output_file(file_out);
    if(!output_file.is_open()) return false;

    Invoice_ChainNode *curr = inv_num_head;
    while(curr) {
        Session_Invoice *temp = curr->invoice;
        tm *subtime = temp->getSubTime();
        tm *servdate = temp->getServDate();

        output_file << temp->getSerNum() << "," << temp->getProNum() << "," << temp->getMemNum() << ","
                << subtime->tm_year << "," << subtime->tm_mon << ","<< subtime->tm_mday << ","
                << subtime->tm_hour << "," << subtime->tm_min << ","<< subtime->tm_sec << ","
                << servdate->tm_year << ","<< servdate->tm_mon << ","<< servdate->tm_mday << ","
                << temp->getComments() << endl;

        curr = curr->inv_num_next;
    }

    output_file.close();
    return true;
}

int Invoice_Chain::getInvoiceCnt() { return ++invoice_count; }