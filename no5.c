#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <unistd.h>

//用户结构体
typedef struct user{
    int bookLent;         //外借的书本数目
    char userName[10];    //用户名字，限定10字
    int bookOwnId[5];     //所借书本的id
    char bookOwn[5][30];  //所借书本的名字(限五本)
    struct user *next;
}user;

//管理员结构体
typedef struct admin{
    char adminName[10];   //管理员名字，限定10字
    struct admin *next;
}admin;

//书本结构体
typedef struct book{
    int id;               //书本id
    char name[30];        //书本名字，限定30字
    char author[10];      //书本作者
    char date[10];        //书本出版日期
    int isLent;           //是否被借走
    char borrowedBy[10];  //借书用户的名字
    struct book *next;
}book;

user *userHead = NULL, *userEnd = NULL;
admin *adminHead = NULL, *adminEnd = NULL;
book *bookHead = NULL, *bookEnd = NULL;
int haveUser = 0, haveAdmin = 0, haveBook = 0; //判断是否第一次添加
int curBookId = 1; //当前应被分配的书本id

void fileRead();
void adminLogin(char *name);
void adminSys();
void userLogin(char *name);
void userSys(user *up);
void addBook();
void deleteBook();
void addUser();
void deleteUser();
void borrowBook(user *up);
void returnBook(user *up);
void showUserBook(user *up);
void deleteUserBook(int id);
void searchUser();
void searchBook();
void editBook();
void editBookSel(book *bp);
void fileSave();


int main(int argc, char **argv)
{
    //获取命令行参数
    int opt = getopt(argc, argv, "a:u:");
    fileRead(); //读取存储文件
    Sleep(1000); //制造好像在加载的错觉
    switch(opt){
        case 'a':
            if(strlen(optarg)>10)
                puts("The name is too long.");
            else{
                adminLogin(optarg);
            }
            break;
        case 'u':
            if(strlen(optarg)>10)
                puts("The name is too long.");
            else{
                userLogin(optarg);
            }
            break;
        default:
            break;
    }
    fileSave(); //存储数据
    return 0;
}

void fileRead(){
    FILE *fp;
    user *up; admin *ap; book *bp;

    userHead = (user*)malloc(sizeof(user));
    userHead->next = NULL;
    up = userHead;
    userEnd = userHead;

    adminHead = (admin*)malloc(sizeof(admin));
    adminHead->next = NULL;
    ap = adminHead;
    adminEnd = adminHead;

    bookHead = (book*)malloc(sizeof(book));
    bookHead->next = NULL;
    bookHead->id = 1;
    bp = bookHead;
    bookEnd = bookHead;

    fp = fopen("./user","r");
    while(fread(up, sizeof(user), 1, fp)){ //fread未读取到时返回0
        if(!haveUser) haveUser = 1; //文件非空时，表明有用户数据
        if(up->next !=NULL){ //分配内存空间储存读取到的数据
            up = (user*)malloc(sizeof(user));
            userEnd->next = up;
            userEnd = up;
            userEnd->next = NULL;
        }
    }
    fclose(fp);

    fp = fopen("./admin","r");
    while(fread(ap, sizeof(admin), 1, fp)){
        if(!haveAdmin) haveAdmin = 1;
        if(ap->next !=NULL){
            ap = (admin*)malloc(sizeof(admin));
            adminEnd->next = ap;
            adminEnd = ap;
            adminEnd->next = NULL;
        }
    }
    fclose(fp);

    fp = fopen("./book","r");
    while(fread(bp, sizeof(book), 1, fp)){
        if(!haveBook) haveBook = 1;
        if(bp->next !=NULL){
            bp = (book*)malloc(sizeof(book));
            bookEnd->next = bp;
            bookEnd = bp;
            bookEnd->next = NULL;
        }
    }
    curBookId = bookEnd->id + haveBook; //用已有书的最大id加1做新书id
    fclose(fp);
}

void userLogin(char *name){
    user *p;
    int isExist = 0;
    puts("login..");

    p = userHead;
    while(p != NULL && haveUser != 0){        //确定是否新用户
        if(strcmp(p->userName, name) == 0){
            puts("existing user...");
            isExist = 1;
            break;
        }
        p = p->next;
    }

    if(!isExist){
        if(haveUser == 0){       //首次添加
            puts("new user..");
            haveUser = 1;
            strcpy(userHead->userName, name);
            userHead->bookLent = 0;
            userHead->next = NULL;
        }
        else{
            puts("new user...");
            p = (user*)malloc(sizeof(user));
            strcpy(p->userName, name);
            p->bookLent = 0;
            userEnd->next = p;
            userEnd = p;
            userEnd->next = NULL;
        }
    }
    userSys(p);
}

void userSys(user *up){
    int cmd;
    do{
        system("cls");

        printf("\n\n\n\n\n");
        printf("\nLibrary Simulator\n");
        printf("1.Borrow book\n");
        printf("2.Return book\n");
        printf("3.My book\n");
        printf("4.Search book\n");
        printf("0.Exit\n\n");
        printf("Please input the command:");
        scanf("%d", &cmd);
        getchar(); //读取换行符
        switch(cmd){
            case 1:
                if(up->bookLent == 5)
                    printf("You can't borrow more!");
                else
                    borrowBook(up);
                break;
            case 2:
                returnBook(up);
                break;
            case 3:
                showUserBook(up);
                break;
            case 4:
                searchBook(up);
                break;
            default:
                break;
        }
    }while(cmd!=0);
}

void adminLogin(char *name){
    admin *p;
    int isExist = 0;
    puts("login..");

    p = adminHead;
    while(p != NULL && haveAdmin != 0){
        if(strcmp(p->adminName, name) == 0){
            puts("existing...");
            isExist = 1;
        }
        p = p->next;
    }

    if(!isExist){
        if(haveAdmin == 0){
            puts("creating..");
            haveAdmin = 1;
            strcpy(adminHead->adminName, name);
            adminHead->next = NULL;
        }
        else{
            puts("making new...");
            p = (admin*)malloc(sizeof(admin));
            strcpy(p->adminName, name);
            adminEnd->next = p;
            adminEnd = p;
            adminEnd->next = NULL;
        }
    }
    adminSys(p);
}

void adminSys(){
    int cmd;
    do{
        system("cls");
        Sleep(500);

        printf("\n\n\n\n\n");
        printf("\nLibrary Simulator Admin\n");
        printf("1.Add book\n");
        printf("2.Delete book\n");
        printf("3.Search book\n");
        printf("4.Edit book\n");
        printf("5.Search user\n");
        printf("6.Add user\n");
        printf("7.Delete user\n");
        printf("0.Exit\n\n");
        printf("Please input the command:");
        scanf("%d", &cmd);
        getchar();
        switch(cmd){
            case 1:
                addBook();
                break;
            case 2:
                deleteBook();
                break;
            case 3:
                searchBook();
                break;
            case 4:
                editBook();
                break;
            case 5:
                searchUser();
                break;
            case 6:
                addUser();
                break;
            case 7:
                deleteUser();
                break;
            default:
                break;
        }
    }while(cmd!=0);
}

void addBook(){
    book *bp;
    char bookName[30];

    if(haveBook){ //第一次添加处理
        bp = (book*)malloc(sizeof(book));
        bookEnd->next = bp;
        bookEnd = bp;
        bookEnd->next = NULL;
    } else{
        haveBook = 1;
        bp = bookHead;
    }

    system("cls");
    Sleep(500);
    printf("Book name:");
    while(gets(bookName)){
        if(strlen(bookName) > 30)
            printf("The name is too long!");
        else
            strcpy(bp->name, bookName);
        break;
    }

    bp->id = curBookId++;
    printf("Author:");
    gets(bp->author);
    printf("Release date(year/month/day):");
    gets(bp->date);
    bp->isLent = 0;

    printf("Book added.");
    Sleep(2000);
}

void deleteBook(){
    book *bp;
    int id, exist = 0;
    char bookName[30], ch;

    printf("Please input the name of the book:");
    gets(bookName);

    bp = bookHead;
    while(bp!= NULL){
        if(strcmp(bookName, bp->name) == 0){ //名字检索
            if(!exist) exist = 1;
            printf("\nID: %12d  Name: %s\n",bp->id, bp->name);
            printf("Author: %8s  Date: %s\n\n",bp->author, bp->date);
            if(bp->isLent)
                printf("LENT to [%s] .", bp->borrowedBy);
        }
        bp = bp->next;
    }
    if(exist){
        printf("Which do you want to delete? Input the id:");

        exist = 0;
        while(!exist){
            bp = bookHead;
            scanf("%d", &id); getchar();
            while(bp != NULL){
                if(id == bp->id){  //id检索
                    exist = 1;
                    break;
                }
                bp = bp->next;
            }
            if(!exist)
                printf("Nonexistent id, input again:");
        }

        do {
            printf("Are you sure you want to delete   ");
            printf("%d : %s (y/n)?   >>>",bp->id,bp->name); //确认过程

            ch = getchar();
            getchar();
            switch (ch) {
                case 'y':
                    deleteUserBook(id);  //删除有此书的用户的所持书信息
                    if(bookHead->next == NULL){  //只有一本时
                        userHead = NULL;
                    }
                    else if(bookHead->id == id){ //删除第一本时
                        bookHead = bookHead->next;
                    }
                    else{
                        while(bp->next != NULL){
                            if(bp->next->id == id)
                                break;
                            bp = bp->next;
                        }
                        bp->next = bp->next->next; //链表删除节点
                    }
                    Sleep(2000);
                    break;
                case 'n':
                    break;
                default:
                    printf("Wrong command!");
                    break;
            }
        }while(ch!='y'&& ch!='n');
    }
    else{
        printf("No such book.");
        Sleep(2000);
    }
}

void addUser(){
    user *up;
    char userName[10];

    if(haveUser){  //首次处理
        up = (user*)malloc(sizeof(user));
        userEnd->next = up;
        userEnd = up;
        userEnd->next = NULL;
    } else{
        haveUser = 1;
        up = userHead;
    }

    system("cls");
    Sleep(500);
    printf("User name:");
    while(gets(userName)){
        if(strlen(userName) > 10)
            printf("The name is too long!");
        else
            strcpy(up->userName, userName);
        break;
    }
    up->bookLent = 0;

    printf("User added.");
    Sleep(2000);
}

void searchUser(){
    user *up;
    char userName[10];
    int exist = 0;

    up = userHead;
    system("cls");
    Sleep(500);

    printf("\nInput the user name:");
    gets(userName);

    while(up != NULL){
        if(strcmp(up->userName, userName) == 0){
            exist = 1;
            break;
        }
        up = up->next;
    }
    if(!exist){
        printf("This user does not exist.\n");
        printf("Press enter to return.");
        getchar();
    }
    else{
        system("cls");
        Sleep(500);

        printf("User [%s] 's profile:\n", up->userName);
        printf("Name:");
        puts(up->userName);
        printf("Book(s)lent:");
        printf("%d\n",up->bookLent);
        if(up->bookLent > 0){
            for (int i = 0; i < up->bookLent; i++){
                printf("ID: %12d  Name: '%s'\n\n",up->bookOwnId[i], up->bookOwn[i]);
            }
        }
        printf("Press enter to return.");
        getchar();
    }
}

void deleteUser(){
    user *up;
    book *bp;
    char userName[10],ch;
    int exist = 0;

    up = userHead;
    system("cls");
    Sleep(500);

    printf("\nInput the user name:");
    gets(userName);

    while(up != NULL){
        if(strcmp(up->userName, userName) == 0){
            exist = 1;
            break;
        }
        up = up->next;
    }
    if(!exist){
        printf("This user does not exist.\n");
        printf("Press enter to return.");
        getchar();
    }
    else{
        system("cls");
        Sleep(500);

        printf("User [%s] 's profile:\n", up->userName);
        printf("Name:");
        puts(up->userName);
        printf("Book(s)lent:");
        printf("%d\n",up->bookLent);
        if(up->bookLent > 0){
            for (int i = 0; i < up->bookLent; i++){
                printf("ID: %12d  Name: '%s'\n\n",up->bookOwnId[i], up->bookOwn[i]);
            }
        }
        do {
            printf("Are you sure you want to delete   ");
            printf("user : [%s] (y/n)?   >>>",up->userName);

            ch = getchar();
            getchar();
            switch (ch) {
                case 'y':
                    for(int i=0;i<up->bookLent;i++){  //归还用户所借的书
                        bp = bookHead;
                        while(bp != NULL){
                            if(up->bookOwnId[i] == bp->id){
                                bp->isLent = 0;
                                break;
                            }
                            bp = bp->next;
                        }
                    }
                    if(userHead->next == NULL){
                        userHead = NULL;
                    }
                    else if(userHead->userName == userName){
                        userHead = userHead->next;
                    }
                    else{
                        while(up->next != NULL){

                            if(up->next->userName == userName)
                                break;
                            up = up->next;
                        }
                        bp->next = bp->next->next;
                    }
                    Sleep(2000);
                    break;
                case 'n':
                    break;
                default:
                    printf("Wrong command!");
                    break;
            }
        }while(ch!='y'&& ch!='n');

        printf("User deleted.");
    }
}

void deleteUserBook(int id){
    book *bp = bookHead;
    user *up = userHead;
    int bookIndex, exist = 0;

    while(bp != NULL){
        if(id == bp->id){
            break;
        }
        bp = bp->next;
    }
    bp->isLent = 0;
    while(up != NULL){  //书本内有关是否被借走的数据的处理
        if(strcmp(up->userName, bp->borrowedBy) == 0){
            for(bookIndex = 0; bookIndex < up->bookLent; bookIndex++){
                if(id == up->bookOwnId[bookIndex]){
                    exist = 1;
                    break;
                }
            }
            break;
        }
        up = up->next;
    }
    if(exist){ //删除用户借书目录的数据
        for(;bookIndex < up->bookLent-1; bookIndex++){
            up->bookOwnId[bookIndex] = up->bookOwnId[bookIndex + 1];
            strcpy(up->bookOwn[bookIndex], up->bookOwn[bookIndex + 1]);
        }
        up->bookLent--;
    }
    printf("User Book Deleted.");
    Sleep(2000);
}

void borrowBook(user *up){
    book *bp;
    int id, exist = 0;
    char bookName[30], ch;

    printf("Please input the name of the book:");
    gets(bookName);

    printf("bookname:  \n%s\n",bookHead->name);

    bp = bookHead;
    while(bp!= NULL){
        if(strcmp(bookName, bp->name) == 0){
            if(!exist) exist = 1;
            printf("\nID: %12d  Name: %s\n",bp->id, bp->name);
            printf("Author: %8s  Date: %s\n",bp->author, bp->date);
            if(bp->isLent)
                printf("LENT to [%s] .", bp->borrowedBy);
        }
        bp = bp->next;
    }
    if(exist){
        printf("Which do you want to borrow? Input the id:");

        exist = 0;
        while(!exist){
            bp = bookHead;
            scanf("%d", &id); getchar();
            while(bp != NULL){
                if(id == bp->id){
                    exist = 1;
                    break;
                }
                bp = bp->next;
            }
            if(!exist)
                printf("Nonexistent id, input again:");
        }

        do {
            printf("Are you sure you want to borrow   ");
            printf("%d : %s (y/n)?   >>>",bp->id,bp->name);

            ch = getchar();
            getchar();
            switch (ch) {
                case 'y':
                    if(bp->isLent){ //假如已被借走了
                        printf("The book is already lent to %s .", bp->borrowedBy);
                        printf("Press enter to go back.");
                        getchar();
                        Sleep(2000);
                    }
                    else{
                        bp->isLent = 1;
                        strcpy(bp->borrowedBy,up->userName);

                        up->bookOwnId[up->bookLent] = bp->id;
                        strcpy(up->bookOwn[up->bookLent],bp->name);
                        up->bookLent++;
                        printf("Book Borrowed.");
                        Sleep(2000);
                    }
                    break;
                case 'n':
                    break;
                default:
                    printf("Wrong command!");
                    break;
            }
        }while(ch!='y'&& ch!='n');
    }
    else{
        printf("No such book.");
        Sleep(2000);
    }
}

void returnBook(user *up){
    book *bp;
    int userBookNum = up->bookLent, bookIndex;
    int exist = 0,id;
    char ch;

    system("cls");
    Sleep(500);
    bp = bookHead;

    if(userBookNum == 0) {
        printf("You have no book.");
        Sleep(2000);
    }
    else{
        printf("Your book:\n");
        while(userBookNum--){
            printf("%d : %s\n", up->bookOwnId[userBookNum], up->bookOwn[userBookNum]);
        }
        printf("\nInput the book id:");
        while(scanf("%d%*c",&id)){
            for(bookIndex = 0; bookIndex < up->bookLent; bookIndex++){
                if(id == up->bookOwnId[bookIndex]){ //这里获取书在用户列表里的索引
                    exist = 1;
                    break;
                }
            }
            if(!exist){
                printf("You don't have this book.\n");
                printf("Please Input again:");
            } else
                break;
        }
        while(bp != NULL){
            if(bp->id == id)
                break;
            bp = bp->next;
        }

        do {
            printf("Are you sure you want to return   ");
            printf("%d : %s (y/n)?   >>>",bp->id,bp->name);

            ch = getchar();
            getchar();
            switch (ch) {
                case 'y':
                    bp->isLent = 0;

                    for(;bookIndex < up->bookLent-1; bookIndex++){
                        up->bookOwnId[bookIndex] = up->bookOwnId[bookIndex + 1];
                        strcpy(up->bookOwn[bookIndex], up->bookOwn[bookIndex + 1]);
                    }
                    up->bookLent--;
                    printf("Book Returned.");
                    Sleep(2000);
                    break;
                case 'n':
                    break;
                default:
                    printf("Wrong command!");
                    break;
            }
        }while(ch!='y'&& ch!='n');
    }
}

void showUserBook(user *up){
    int userBookNum = up->bookLent;
    int n;
    char ch;

    system("cls");
    Sleep(500);
    printf("\nBooklisk:\n");
    if(userBookNum == 0)
        printf("No books.");
    while(userBookNum--){
        printf("\n%d : %s\n\n", up->bookOwnId[userBookNum], up->bookOwn[userBookNum]);
    }

    printf("Press enter to go back.");
    getchar();
}

void searchBook(){
    book *bp;
    char bookName[30], ch;
    int exist = 0, id, cmd;
    bp = bookHead;

    system("cls");
    Sleep(500);
    printf("\n1.search by name\n");
    printf("\n2.search by id\n");
    printf("\n0.return\n");
    printf("\nPlease input the command:");

    scanf("%d",&cmd); getchar();
    if(cmd == 1){
        printf("\nInput the name:");
        gets(bookName);

        while(bp != NULL){
            if(strcmp(bookName, bp->name) == 0){
                if(!exist) exist = 1;
                printf("\n\nID: %12d  Name: %s\n",bp->id, bp->name);
                printf("Author: %8s  Date: %s\n\n",bp->author, bp->date);
                if(bp->isLent)
                    printf("LENT to [%s] .", bp->borrowedBy);
            }
            bp = bp->next;
        }
        if(!exist)
            printf("No such book!");
    }
    else if(cmd == 2){
        printf("Input the id:");

        bp = bookHead;
        scanf("%d", &id); getchar();
        while(bp != NULL){
            if(id == bp->id){
                exist = 1;
                break;
            }
            bp = bp->next;
        }
        if(exist) {
            printf("\n\nID: %12d  Name: %s\n", bp->id, bp->name);
            printf("Author: %8s  Date: %s\n\n", bp->author, bp->date);
            if(bp->isLent)
                printf("LENT to [%s] .", bp->borrowedBy);
        }
        else{
            printf("No such book!");
        }
    }

    printf("Press enter to go back.");
    getchar();
}

void editBook(){
    book *bp;
    int id, exist = 0;
    char bookName[30], ch;

    printf("Please input the name of the book:");
    gets(bookName);

    printf("bookname:  \n%s\n",bookHead->name);

    bp = bookHead;
    while(bp!= NULL){
        if(strcmp(bookName, bp->name) == 0){
            if(!exist) exist = 1;
            printf("\nID: %12d  Name: %s\n",bp->id, bp->name);
            printf("Author: %8s  Date: %s\n\n",bp->author, bp->date);
            if(bp->isLent)
                printf("LENT to [%s] .", bp->borrowedBy);
        }
        bp = bp->next;
    }
    if(exist){
        printf("Which do you want to edit? Input the id:");

        exist = 0;
        while(!exist){
            bp = bookHead;
            scanf("%d", &id); getchar();
            while(bp != NULL){
                if(id == bp->id){
                    exist = 1;
                    break;
                }
                bp = bp->next;
            }
            if(!exist)
                printf("Nonexistent id, input again:");
        }

        do {
            printf("Are you sure you want to edit   ");
            printf("%d : %s (y/n)?   >>>",bp->id,bp->name);

            ch = getchar();
            getchar();
            switch (ch) {
                case 'y':
                    editBookSel(bp); //切到具体选项
                    break;
                case 'n':
                    break;
                default:
                    printf("Wrong command!");
                    break;
            }
        }while(ch!='y'&& ch!='n');
    }
    else{
        printf("No such book.");
        Sleep(2000);
    }
}

void editBookSel(book *bp){
    int cmd;
    char bookName[30];

    do {
        system("cls");

        printf("1.Edit name\n");
        printf("2.Edit id\n");
        printf("3.Edit release date\n");
        printf("4.Edit Author\n");
        printf("5.Force return\n");
        printf("0.Exit\n");

        scanf("%d", &cmd);
        getchar();
        switch (cmd) {
            case 1:
                printf("Book name:");
                while(gets(bookName)){
                    if(strlen(bookName) > 30)
                        printf("The name is too long!");
                    else
                        strcpy(bp->name, bookName);
                    break;
                }
                break;
            case 2:
                printf("ID:");
                scanf("%d%*c",&bp->id);
                break;
            case 3:
                printf("Release date(year/month/day):");
                gets(bp->date);
                break;
            case 4:
                printf("Author:");
                gets(bp->author);
                break;
            case 5:
                bp->isLent = 0;
                break;
            default:
                break;
        }
    }while(cmd != 0);
}

void fileSave(){
    FILE *fp;
    user *up; admin *ap; book *bp;
    up = userHead; ap = adminHead; bp = bookHead;

    fp = fopen("./user", "w");
    while(up !=NULL && haveUser){
        fwrite(up, sizeof(user), 1, fp);
        up = up->next;
    }
    fclose(fp);

    fp = fopen("./admin", "w");
    while(ap !=NULL && haveAdmin){
        fwrite(ap, sizeof(admin), 1, fp);
        ap = ap->next;
    }
    fclose(fp);

    fp = fopen("./book", "w");
    while(bp != NULL){
        fwrite(bp, sizeof(book), 1, fp);
        bp = bp->next;
    }
    fclose(fp);
}