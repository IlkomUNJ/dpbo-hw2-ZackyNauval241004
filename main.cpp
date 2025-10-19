#include <iostream>
#include <string>
#include <limits>
#include "OnlineStore.h"

void pause(){ std::cout << "Tekan enter..."; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); }

int main(){
    OnlineStore store;
    store.load();
    int choice=0;
    while(true){
        std::cout << "=== Online Store Sim ===\n";
        std::cout << "1. Register buyer\n2. Register seller\n3. Create bank account for user\n4. Topup account\n5. Add item (seller)\n6. Create order (buyer)\n7. Pay order\n8. Complete order\n9. List transactions last k days\n10. List paid not completed\n11. Bank: transactions last week\n12. Bank: dormant accounts\n13. Stats: top m items\n14. Stats: active buyers today\n0. Exit\nChoice: ";
        if(!(std::cin >> choice)) break;
        if(choice==0) break;
        if(choice==1){
            std::string name; std::cout<<"username: "; std::cin>>name;
            auto *u = store.registerUser(name, Role::BUYER);
            if(u) std::cout<<"Buyer registered id="<<u->id<<"\n"; else std::cout<<"username exists\n";
        } else if(choice==2){
            std::string name; std::cout<<"username: "; std::cin>>name;
            auto *u = store.registerUser(name, Role::SELLER);
            if(u) std::cout<<"Seller registered id="<<u->id<<"\n"; else std::cout<<"username exists\n";
        } else if(choice==3){
            int uid; std::cout<<"user id: "; std::cin>>uid;
            auto *u = store.findUserById(uid);
            if(!u){ std::cout<<"user not found\n"; continue; }
            if(u->bankAccountId!=-1) { std::cout<<"already has account id="<<u->bankAccountId<<"\n"; continue; }
            auto *acc = store.bank.createAccount(u->id);
            if(acc){ u->bankAccountId = acc->id; store.save(); std::cout<<"account created id="<<acc->id<<"\n"; }
        } else if(choice==4){
            int uid; double amt; std::cout<<"user id: "; std::cin>>uid; std::cout<<"amount: "; std::cin>>amt;
            auto *u = store.findUserById(uid);
            if(!u || u->bankAccountId==-1){ std::cout<<"user/no account\n"; continue;}
            if(store.bank.topup(u->bankAccountId, amt)) { std::cout<<"topup success\n"; store.save(); } else std::cout<<"failed\n";
        } else if(choice==5){
            int sid; std::cout<<"seller id: "; std::cin>>sid;
            auto *s = store.findUserById(sid);
            if(!s || s->role!=Role::SELLER){ std::cout<<"seller not found\n"; continue;}
            std::string name; int stock; double price;
            std::cout<<"item name: "; std::cin>>name; std::cout<<"stock: "; std::cin>>stock; std::cout<<"price: "; std::cin>>price;
            auto *it = store.addItem(sid, name, stock, price);
            std::cout<<"item added id="<<it->id<<"\n";
        } else if(choice==6){
            int bid, itemid, q; std::cout<<"buyer id: "; std::cin>>bid; std::cout<<"item id: "; std::cin>>itemid; std::cout<<"qty: "; std::cin>>q;
            auto *t = store.createOrder(bid, itemid, q);
            if(t) std::cout<<"order created txId="<<t->id<<"\n"; else std::cout<<"failed (stock or ids)\n";
        } else if(choice==7){
            int tx; std::cout<<"tx id: "; std::cin>>tx;
            if(store.payOrder(tx)) std::cout<<"paid\n"; else std::cout<<"pay failed\n";
        } else if(choice==8){
            int tx; std::cout<<"tx id: "; std::cin>>tx;
            if(store.completeOrder(tx)) std::cout<<"completed\n"; else std::cout<<"complete failed\n";
        } else if(choice==9){
            int k; std::cout<<"k days: "; std::cin>>k;
            auto v = store.listTransactionsLastKDays(k);
            for(auto &t: v) std::cout<<"tx "<<t.id<<" buyer "<<t.buyerId<<" seller "<<t.sellerId<<" amt "<<t.amount<<" status "<<Transaction::statusToChar(t.status)<<"\n";
        } else if(choice==10){
            auto v = store.listPaidNotCompleted();
            for(auto &t: v) std::cout<<"tx "<<t.id<<" buyer "<<t.buyerId<<" seller "<<t.sellerId<<" amt "<<t.amount<<"\n";
        } else if(choice==11){
            auto v = store.bank.transactionsLastWeek();
            for(auto &t: v) std::cout<<"bank-tx "<<t.id<<" txRefBuyer "<<t.buyerId<<" amt "<<t.amount<<"\n";
        } else if(choice==12){
            auto v = store.bank.dormantAccounts();
            for(auto a: v) std::cout<<"acc "<<a->id<<" owner "<<a->ownerUserId<<" last "<<a->lastActivity<<"\n";
        } else if(choice==13){
            int m; std::cout<<"m: "; std::cin>>m;
            auto v = store.topMItems(m);
            for(auto id: v){ auto it = store.findItemById(id); if(it) std::cout<<"item "<<it->id<<" name "<<it->name<<" sold "<<it->soldCount<<"\n"; }
        } else if(choice==14){
            auto v = store.mostActiveBuyersPerDay(5);
            std::cout<<"top buyers today:\n";
            for(auto id: v) { auto u = store.findUserById(id); if(u) std::cout<<u->id<<" "<<u->username<<"\n"; }
        } else {
            std::cout<<"invalid\n";
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        pause();
    }
    store.save();
    std::cout<<"bye\n";
    return 0;
}
