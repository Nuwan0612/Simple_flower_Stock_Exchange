#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <unordered_map>
#include <chrono>
#include <bits/stdc++.h>

using namespace std;

//class related to the order
class TradeOrder{
    public:
        string client_order_id;
        string instrument;
        string order_id;
        string status;
        int side;
        double price;
        int quantity;
        string reason;
        string transaction_time;

        TradeOrder(){
            client_order_id = "";
            instrument = "";
            order_id = "";
            status = "";
            side = 0;
            price = 0;
            quantity = 0;
            transaction_time = "";
            reason = "";
        }
};

//counter to generate the order number and unordered vectors to store the orders
int counter = 0;
unordered_map<string, vector<TradeOrder>> buy;
unordered_map<string, vector<TradeOrder>> sell;

//where the merge happens in merge sort
const int RUN = 32;
 
// This function sorts array from left index to
// to right index which is of size atmost RUN
void insertionSort(vector<TradeOrder>& order, int left, int right, string side)
{
    for (int i = left + 1; i <= right; i++)
    {
        TradeOrder temp = order[i];
        int j = i - 1;
        if (side == "buy"){
            while (j >= left && order[j].price < temp.price)
            {
                order[j+1] = order[j];
                j--;
            }
        }
        else if (side == "sell"){
            while (j >= left && order[j].price > temp.price)
            {
                order[j+1] = order[j];
                j--;
            }
        }
        order[j+1] = temp;
    }
}
 
// Merge function merges the sorted runs
void merge(vector<TradeOrder>& order, int l, int m, int r, string side)
{
     
    // Original array is broken in two parts
    // left and right array
    int len1 = m - l + 1, len2 = r - m;
    TradeOrder left[len1], right[len2];
    for (int i = 0; i < len1; i++)
        left[i] = order[l + i];
    for (int i = 0; i < len2; i++)
        right[i] = order[m + 1 + i];
 
    int i = 0;
    int j = 0;
    int k = l;
 
    // After comparing, we
    // merge those two array
    // in larger sub array
    while (i < len1 && j < len2)
    {
        if (side =="buy"){
            if (left[i].price >= right[j].price)
            {
                order[k] = left[i];
                i++;
            }
            else
            {
                order[k] = right[j];
                j++;
            }
        }
        else if (side =="sell"){
            if (left[i].price <= right[j].price)
            {
                order[k] = left[i];
                i++;
            }
            else
            {
                order[k] = right[j];
                j++;
            }
        }
        k++;
    }
 
    // Copy remaining elements of left, if any
    while (i < len1)
    {
        order[k] = left[i];
        k++;
        i++;
    }
 
    // Copy remaining element of right, if any
    while (j < len2)
    {
        order[k] = right[j];
        k++;
        j++;
    }
}
 
// Iterative Timsort function to sort the
// array[0...n-1] (similar to merge sort)
void timSort(vector<TradeOrder>& order, int n, string side)
{
     
    // Sort individual subarrays of size RUN
    for (int i = 0; i < n; i+=RUN)
        insertionSort(order, i, min((i+RUN-1), (n-1)),side);
 
    // Start merging from size RUN (or 32).
    // It will merge
    // to form size 64, then 128, 256
    // and so on ....
    for (int size = RUN; size < n;
                             size = 2*size)
    {
         
        // pick starting point of
        // left sub array. We
        // are going to merge
        // order[left..left+size-1]
        // and order[left+size, left+2*size-1]
        // After every merge, we
        // increase left by 2*size
        for (int left = 0; left < n;
                             left += 2*size)
        {
             
            // find ending point of
            // left sub array
            // mid+1 is starting point
            // of right sub array
            int mid = left + size - 1;
            int right = min((left + 2*size - 1),
                                            (n-1));
 
            // merge sub array order[left.....mid] &
            // order[mid+1....right]
              if(mid < right)
                merge(order, left, mid, right, side);
        }
    }
}


//check priority
void check_priority(TradeOrder new_order, unordered_map<string, vector<TradeOrder>>& order,string order_type){

    vector<TradeOrder>& orders = order[new_order.instrument];
    if(orders.size() == 0 || (order_type == "sell" && new_order.price < orders[0].price) || (order_type == "buy" && new_order.price > orders[0].price)){
        orders.insert(orders.begin(), new_order);
    }
    else if((order_type == "sell" && new_order.price >= orders[orders.size()-1].price) || (order_type == "buy" && new_order.price <= orders[orders.size()-1].price)){
        orders.push_back(new_order);
    }
    else{ 
        orders.push_back(new_order);
        int n = orders.size();
        timSort(orders,n,order_type);
    }  
}

//check the validity of an order
int check_validity(TradeOrder &new_order){

    int valid_count = 0;

    transform(new_order.instrument.begin(), new_order.instrument.end(), new_order.instrument.begin(), ::tolower);
    if((new_order.instrument == "rose") || (new_order.instrument == "lavender") || (new_order.instrument == "lotus") || (new_order.instrument == "tulip") || (new_order.instrument == "orchid")){
        valid_count++;
        //convert first character to uppercase
        new_order.instrument[0] = std::toupper(new_order.instrument[0]); // convert first character to uppercase
        new_order.instrument = new_order.instrument.substr(0, 1) + new_order.instrument.substr(1);
    }else{
        new_order.reason = "Invalid instrument";
    }
    if(new_order.side == 1 || new_order.side == 2){
        valid_count++;
    }else{
        new_order.reason = "Invalid side";
    }
    if((new_order.quantity >= 10 && new_order.quantity <= 1000) && (new_order.quantity%10 == 0 )){
        valid_count++;
    }else{
        new_order.reason = "Invalid quantity";
    }
    if(new_order.price > 0){
        valid_count++;
    }else{
        new_order.reason = "Invalid price";
    }

    return (valid_count == 4)? 1 : 0;
}

//writes the transaction and order details to csv file that was created by the main function
void display(TradeOrder new_order){
    ofstream file("execution_rep.csv", ios::app);
    if(file.is_open()){
        file << "ord" << new_order.order_id << ","
             << new_order.client_order_id << ","
             << new_order.instrument << ","
             << new_order.side << ","
             << new_order.status << ","
             << new_order.quantity << ","
             << fixed << setprecision(2) << new_order.price << ","
             << new_order.transaction_time << ","
             <<  new_order.reason << endl;
    }
    file.close();
}

//transaction logic
void exchange(vector<TradeOrder>& vector_buy, vector<TradeOrder>& vector_sell, TradeOrder &new_order){

    int flag = 0;

    while((vector_buy.size() != 0 && vector_sell.size() != 0) && (vector_sell[0].price <= vector_buy[0].price) && (vector_sell[0].instrument == vector_buy[0].instrument))
    {
        flag = 1;
        if(vector_sell[0].quantity == vector_buy[0].quantity)
        {
            vector_sell[0].price = vector_buy[0].price;
            vector_sell[0].status = "Fill";
            vector_buy[0].status = "Fill";

            display(vector_sell[0]);
            display(vector_buy[0]);

            vector_buy.erase(vector_buy.begin());
            vector_sell.erase(vector_sell.begin());
        }
        else if(vector_sell[0].quantity < vector_buy[0].quantity)
        {
            vector_sell[0].price = vector_buy[0].price;
            vector_sell[0].status = "Fill";
            vector_buy[0].status = "PFill";
            int remain = vector_buy[0].quantity - vector_sell[0].quantity;
            vector_buy[0].quantity = vector_sell[0].quantity;

            display(vector_sell[0]);
            display(vector_buy[0]);

            vector_sell.erase(vector_sell.begin());
            vector_buy[0].quantity = remain;
            //vector_buy[0].status = "Pending";

            //if(vector_sell.size() == 0 || (vector_sell[0].price > vector_buy[0].price)){
            //    display(vector_buy[0]);
            //}
        }
        else
        {
            double price = vector_sell[0].price;
            int remain =  vector_sell[0].quantity - vector_buy[0].quantity;
            vector_sell[0].quantity = vector_buy[0].quantity;
            vector_sell[0].price = vector_buy[0].price;
            vector_sell[0].status = "PFill";
            vector_buy[0].status = "Fill";

            display(vector_sell[0]);
            display(vector_buy[0]);

            vector_buy.erase(vector_buy.begin());

            vector_sell[0].quantity = remain;
            vector_sell[0].price = price;
           // vector_sell[0].status = "Pending";

           // if(vector_buy.size() == 0 || (vector_sell[0].price > vector_buy[0].price)){
           //     display(vector_sell[0]);
           // }
        }
    }

    if(flag == 0 && new_order.status == "New"){
        display(new_order);
    }
}

//add an order to the order book
void add_order(string client, string instrument, int side, int quantity, double price, string transaction_time){

   TradeOrder new_order;

   new_order.client_order_id = client;
   new_order.instrument = instrument;
   new_order.side = side;
   new_order.quantity = quantity;
   new_order.price = price;
   new_order.status = "New";
   new_order.reason = " ";
   counter = counter + 1;
   new_order.order_id = to_string(counter);
   new_order.transaction_time = transaction_time;

   if(check_validity(new_order)){
        //checking the right vector respect to the order type pass order to the vector
        if(new_order.side == 1){
            check_priority(new_order,buy,"buy");
        }else{
            check_priority(new_order,sell,"sell");
        }
        //do the transactions
        exchange(buy[new_order.instrument], sell[new_order.instrument], new_order);
   }else{
       new_order.status = "Reject";
       display(new_order);
   }
}

//get the input from input csv file and creates the output csv file with headers
int main(){

    ifstream input_file;
	input_file.open("orderbook.csv");   //need to give the location of the input file. e.g.: input_file.open("D:\\Folder name\\File_Name.csv");

    //creating the output csv file
    ofstream file("execution_rep.csv");    //need to give the location for the output. e.g.: ofstream file("D:\\Folder name\\File_Name.csv");
    if(file.is_open()){
        file << "Order ID" << ","
             << "Client Order" << ","
             << "Instrument" << ","
             << "Side" << ","
             << "Exec Status" << ","
             << "Quantity" << ","
             << "Price" << ","
             << "Transaction Time"<<","
             << "Reason" << endl;
    }
    file.close();

	string line = "";

	getline(input_file,line);
	line = "";

    //read the data from the input csv file and pass those data to the process
	while(getline(input_file, line)){
		string client;
        string instrument;
        int side;
        int quantity;
        double price;
        string transaction_time;
		string temp_string = "";

		stringstream input_string(line);

		getline(input_string, client, ',');
		getline(input_string, instrument, ',');;

		getline(input_string, temp_string, ',');
		side = atoi(temp_string.c_str());

        temp_string = "";
		getline(input_string, temp_string, ',');
		quantity = atoi(temp_string.c_str());

		temp_string = "";
		getline(input_string, temp_string, ',');
		price = atof(temp_string.c_str());

        auto now = chrono::system_clock::now();
        auto now_c = chrono::system_clock::to_time_t(now);
        struct tm* parts = localtime(&now_c);
        char timestamp[20];
        sprintf(timestamp, "%04d%02d%02d-%02d%02d%02d.000", parts->tm_year + 1900, parts->tm_mon + 1,
        parts->tm_mday, parts->tm_hour, parts->tm_min, parts->tm_sec);
        transaction_time = timestamp;

        add_order(client, instrument, side, quantity, price, transaction_time); //pass the details for the process

		line = "";
	}

}
