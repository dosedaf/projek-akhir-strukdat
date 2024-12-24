#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX_HASH 100
#define MAX_TITLE 100
#define MAX_AUTHOR 100

typedef struct Book {
    char title[MAX_TITLE];
    char author[MAX_AUTHOR];
    int refNumber;
    struct Book* next; // chaining
} Book;

// node ptb
typedef struct TreeNode {
    Book* book;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

typedef struct History {
    Book* deletedBooks[MAX_HASH];
    int count;
} History;

// buat delay
void delay(int number_of_seconds)
{
    int milli_seconds = 1000 * number_of_seconds;

    clock_t start_time = clock();

    while (clock() < start_time + milli_seconds)
        ;
}

// init
Book* hashTable[MAX_HASH];
TreeNode* root = NULL;
History history = {.count = 0};

// Fungsi hash sederhana
int hashFunction(int refNumber) {
    return refNumber % MAX_HASH;
}

// buat buku baru
Book* createBook(char* title, char* author, int refNumber) {
    Book* newBook = (Book*)malloc(sizeof(Book));
    strcpy(newBook->title, title); // copy strarg ke book
    strcpy(newBook->author, author);
    newBook->refNumber = refNumber;
    newBook->next = NULL;
    return newBook;
}

// buat node baru
TreeNode* createTreeNode(Book* book) {
    TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
    newNode->book = book;
    newNode->left = newNode->right = NULL;
    return newNode;
}

// insertion n chaining hash table
int insertIntoHash(Book* book) {
    int index = hashFunction(book->refNumber);
    
    Book* current = hashTable[index];
    while (current != NULL) {
        if (current->refNumber == book->refNumber) {
            printf("Error: Nomor Referensi Duplikat!\n");
            return -1;
        }
        current = current->next;
    }
    
    book->next = hashTable[index];
    hashTable[index] = book;
}


// insertion ptb
TreeNode* insertIntoBST(TreeNode* node, Book* book) {
    if (node == NULL) {
        return createTreeNode(book);
    }
    
    if (strcmp(book->title, node->book->title) < 0) {
        node->left = insertIntoBST(node->left, book);
    } else {
        node->right = insertIntoBST(node->right, book);
    }
    return node;
}

// cari min val
TreeNode* minValueNode(TreeNode* node) {
    TreeNode* current = node;
    while (current && current->left != NULL)
        current = current->left;
    return current;
}

// deletion ptb
TreeNode* deleteFromBST(TreeNode* root, int refNumber) {
    if (root == NULL)
        return root;

    // cari
    if (root->book->refNumber > refNumber) {
        root->left = deleteFromBST(root->left, refNumber);
    }
    else if (root->book->refNumber < refNumber) {
        root->right = deleteFromBST(root->right, refNumber);
    }
    else {
        // anak 1 / < 1
        if (root->left == NULL) {
            TreeNode* temp = root->right;
            free(root);
            return temp;
        }
        else if (root->right == NULL) {
            TreeNode* temp = root->left;
            free(root);
            return temp;
        }

        // anak 2
        TreeNode* temp = minValueNode(root->right);
        root->book = temp->book;
        root->right = deleteFromBST(root->right, temp->book->refNumber);
    }

    return root;
}

// tambah buku, hashmap n ptb
void addBook(char* title, char* author, int refNumber) {
    Book* newBook = createBook(title, author, refNumber);
    int ok = insertIntoHash(newBook);
    if(ok == -1) {
        printf("Buku gagal ditambahkan\n");
        return;
    }
    root = insertIntoBST(root, newBook);
    printf("Buku berhasil ditambahkan!\n");
}

// lookup dari noref
Book* searchByRef(int refNumber) {
    int index = hashFunction(refNumber);
    Book* current = hashTable[index];
    while (current != NULL) {
        if (current->refNumber == refNumber) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// search range rekursif
void searchRange(TreeNode* node, char* start, char* end) {
    if (node == NULL) return;
    
    if (strcmp(node->book->title, start) >= 0 && strcmp(node->book->title, end) <= 0) {
        printf("Judul: %s, Penulis: %s, No.Ref: %d\n", 
               node->book->title, node->book->author, node->book->refNumber);
    }
    
    if (strcmp(node->book->title, start) > 0) {
        searchRange(node->left, start, end);
    }
    if (strcmp(node->book->title, end) < 0) {
        searchRange(node->right, start, end);
    }
}

// inorder traversal atau ascending
void inorderTraversal(TreeNode* node) {
    if (node == NULL) return;
    inorderTraversal(node->left);
    printf("Judul: %s, Penulis: %s, No.Ref: %d\n", 
           node->book->title, node->book->author, node->book->refNumber);
    inorderTraversal(node->right);
}

// preorder traversal
void preorderTraversal(TreeNode* node) {
    if (node == NULL) return;
    printf("Judul: %s, Penulis: %s, No.Ref: %d\n", 
           node->book->title, node->book->author, node->book->refNumber);
    preorderTraversal(node->left);
    preorderTraversal(node->right);
}

// post order traversal
void postorderTraversal(TreeNode* node) {
    if (node == NULL) return;
    postorderTraversal(node->left);
    postorderTraversal(node->right);
    printf("Judul: %s, Penulis: %s, No.Ref: %d\n", 
           node->book->title, node->book->author, node->book->refNumber);
}

// deletion buku
void deleteBook(int refNumber) {
    Book* book = searchByRef(refNumber);
    if (book != NULL) {
        // simpan ke history
        history.deletedBooks[history.count++] = createBook(book->title, book->author, book->refNumber);
        
        // hapus dr hash table
        int index = hashFunction(refNumber);
        Book* toDelete = hashTable[index];
        if (hashTable[index]->refNumber == refNumber) {
            hashTable[index] = hashTable[index]->next;
            free(toDelete);
        } else {
            Book* current = hashTable[index];
            while (current->next != NULL) {
                if (current->next->refNumber == refNumber) {
                    toDelete = current->next;
                    current->next = current->next->next;
                    free(toDelete);
                    break;
                }
                current = current->next;
            }
        }

        // hapus dari ptb
        root = deleteFromBST(root, refNumber);
        
        printf("Buku berhasil dihapus!\n");
    } else {
        printf("Buku tidak ditemukan!\n");
    }
}

// buat history
TreeNode* createHistoryTree() {
    TreeNode* historyRoot = NULL;
    for(int i = 0; i < history.count; i++) {
        Book* book = history.deletedBooks[i];
        historyRoot = insertIntoBST(historyRoot, book);
    }
    return historyRoot;
}

void freeTree(TreeNode* node) {
    if(node == NULL) return;
    freeTree(node->left);
    freeTree(node->right);
    free(node);
}

void freeHashTable() {
    for(int i = 0; i < MAX_HASH; i++) {
        Book* current = hashTable[i];
        while(current != NULL) {
            Book* temp = current;
            current = current->next;
            free(temp);
        };
    };
};

void showHistoryMenu() {
    if(history.count == 0) {
        printf("\nBelum ada buku yang dihapus!\n");
        return;
    }

    int choice;
    TreeNode* historyRoot = createHistoryTree();
    
    while (1) {
        printf("\n========================================\n");
        printf("            Menu Lihat History           \n");
        printf("========================================\n");
        printf("1. Tampilkan History (In-order/Ascending)\n");
        printf("2. Tampilkan History (Pre-order)\n");
        printf("3. Tampilkan History (Post-order)\n");
        printf("4. Kembali ke Menu Utama\n");
        printf("Pilihan: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                printf("\nHistory Buku yang Dihapus (In-order):\n");
                inorderTraversal(historyRoot);
                break;
            case 2:
                printf("\nHistory Buku yang Dihapus (Pre-order):\n");
                preorderTraversal(historyRoot);
                break;
            case 3:
                printf("\nHistory Buku yang Dihapus (Post-order):\n");
                postorderTraversal(historyRoot);
                break;
            case 4:
                freeTree(historyRoot);
                return;
            default:
                printf("Pilihan tidak valid!\n");
        }

        delay(2);
        system("cls");
    }
}


int main() {
    int choice, refNumber;
    char title[MAX_TITLE], author[MAX_AUTHOR];
    char startTitle[MAX_TITLE], endTitle[MAX_TITLE];
    
    // Inisialisasi hash table
    for (int i = 0; i < MAX_HASH; i++) {
        hashTable[i] = NULL;
    }

    system("cls");
    printf("=========================================\n");
    printf("|         Perpustakaan Kakang           |\n");
    printf("=========================================\n");
    
    printf("  %-25s | %-10s\n", "Nama", "NIM");
    printf("-----------------------------------------\n");
    printf("  %-25s | %-10s\n", "Reyhan Nabil Destra", "123230088");
    printf("  %-25s | %-10s\n", "Ayodya Enhanayoan", "123230099");
    printf("  %-25s | %-10s\n", "Andika Herindra Setiawan", "123230117");
    printf("  %-25s | %-10s\n", "Ghiva Satria Widagda", "123230209");
    printf("  %-25s | %-10s\n", "Muhammad Thoriq Aziz", "123230233");
    
    printf("=========================================\n");
    printf("Memindahkan ke halaman selanjutnya dalam 5 detik...\n");
    delay(3);

    while (1) {
        delay(2);
        system("cls");
        printf("\n========================================\n");
        printf("      Sistem Manajemen Perpustakaan     \n");
        printf("========================================\n");
        printf("  1. Tambah Buku                        \n");
        printf("  2. Cari Buku (No. Referensi)          \n");
        printf("  3. Cari Buku (Rentang Judul)          \n");
        printf("  4. Tampilkan Semua Buku (Ascending)   \n");
        printf("  5. Hapus Buku                         \n");
        printf("  6. Tampilkan History                  \n");
        printf("  7. Keluar                             \n");
        printf("========================================\n");
        printf("Pilihan Anda: ");

        scanf("%d", &choice);
        getchar(); // clear buffer
        
        switch (choice) {
            case 1:
                system("cls");
                printf("\n========================================\n");
                printf("             Menu Tambah Buku            \n");
                printf("========================================\n");
                printf("Judul: ");
                fgets(title, MAX_TITLE, stdin);
                title[strcspn(title, "\n")] = 0;
                printf("Penulis: ");
                fgets(author, MAX_AUTHOR, stdin);
                author[strcspn(author, "\n")] = 0;
                printf("No. Referensi: ");
                scanf("%d", &refNumber);
                addBook(title, author, refNumber);
                break;
                
            case 2:
                system("cls");
                printf("\n========================================\n");
                printf("             Menu Cari Buku             \n");
                printf("========================================\n");
                printf("Masukkan No. Referensi: ");
                scanf("%d", &refNumber);
                Book* found = searchByRef(refNumber);
                if (found) {
                    printf("Buku ditemukan:\n");
                    printf("Judul: %s, Penulis: %s, No.Ref: %d\n", 
                           found->title, found->author, found->refNumber);
                } else {
                    printf("Buku tidak ditemukan!\n");
                }
                break;
                
            case 3:
                system("cls");
                printf("\n========================================\n");
                printf("             Menu Cari Buku             \n");
                printf("========================================\n");
                printf("Judul Awal: ");
                fgets(startTitle, MAX_TITLE, stdin);
                startTitle[strcspn(startTitle, "\n")] = 0;
                printf("Judul Akhir: ");
                fgets(endTitle, MAX_TITLE, stdin);
                endTitle[strcspn(endTitle, "\n")] = 0;
                printf("Buku dalam rentang %s - %s:\n", startTitle, endTitle);
                searchRange(root, startTitle, endTitle);
                break;
                
            case 4:
                system("cls");
                printf("\n========================================\n");
                printf("         Daftar Buku (Ascending)        \n");
                printf("========================================\n");
                inorderTraversal(root);
                break;
                
            case 5:
                system("cls");
                printf("\n========================================\n");
                printf("             Menu Hapus Buku           \n");
                printf("========================================\n");
                printf("No. Referensi: ");
                scanf("%d", &refNumber);
                deleteBook(refNumber);
                break;
                
            case 6:
                system("cls");
                showHistoryMenu();
                break;
                
            case 7:
                system("cls");
                printf("\n========================================\n");
                printf("Terimakasih telah menggunakan aplikasi kami\n");
                printf("========================================\n");
                freeHashTable();
                freeTree(root);
                for(int i = 0; i < history.count; i++) {
                    free(history.deletedBooks[i]);
                };
                exit(0);
                
            default:
                printf("Pilihan tidak valid!\n");
        }
    }
    
    return 0;
}
