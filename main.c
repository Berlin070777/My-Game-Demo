#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h> 
#include <unistd.h> 

#define MONSTER_BASE_HP 80
#define MONSTER_BASE_ATK 15
#define PLAYER_BASE_ATK 5
#define SWORD_ATK_BONUS 30     
#define ARMOR_DEF_BONUS 5      
#define JACKPOT_PRIZE 1000     
#define SHIELD_BLOCK_CHANCE 25 

struct Item {
    int id;
    char* name;
    int price;
};

struct Player{
    int gold;
    int hp;
    int *bag;
    int bagSize;
    int maxhp;
};

void Showstatus (int currentgold, int currenthp, int maxhp) {
    printf("\n========================================\n"); 
    printf(" PLAYER STATUS\n");
    printf(" 💰 Gold: %-5d   ❤️  HP: %d/%d\n" , currentgold, currenthp, maxhp);
    printf("========================================\n");
}

int CalTotalPrice (int x , int y) {
    return x * y;
}

void SaveGame(struct Player *p) {
    FILE* fp = fopen("game_save.txt", "w");
    if (fp == NULL) {
        printf("Error: Can't open file to save!\n");
        sleep(1);
        return; 
    }
    fprintf(fp, "%d %d %d %d\n", p->gold, p->hp, p->maxhp, p->bagSize); 
    for (int i = 0; i < p->bagSize; i++) {
        fprintf(fp, "%d ", p->bag[i]);
    }
    fclose(fp);
    printf(">>> Game Saved! <<<\n");
    sleep(1);
}

void LoadGame(struct Player *p) {
    FILE* fp = fopen("game_save.txt", "r");
    if (fp == NULL) {
        printf("Error: No save file found!\n");
        sleep(1);
        return;
    }
    
    if (p->bag != NULL) {
        free(p->bag);
    }

    fscanf(fp, "%d %d %d %d", &p->gold, &p->hp, &p->maxhp, &p->bagSize);
    
    p->bag = (int*)malloc(p->bagSize * sizeof(int));
    if (p->bag == NULL) {
        printf("Memory Error during loading!\n");
        fclose(fp);
        return;
    }

    for (int i = 0; i < p->bagSize; i++) {
        fscanf(fp, "%d", &p->bag[i]);
    }
    fclose(fp);
    printf(">>> Game Loaded! Welcome back! <<<\n");
    sleep(1);
}

void Battle(struct Player *p) {
    char input[50]; 

    printf("\n>>> You venture into the wilds... <<<\n");
    sleep(1); 

    int encounter = rand() % 2; 
    if (encounter == 0) {
        printf("...\n");
        sleep(1);     
        printf("You walked for a while but found nothing.\n");
        printf("You lost 5 HP due to fatigue.\n");
        
        p->hp -= 5;
        if(p->hp < 0) p->hp = 0; 
        
        sleep(2); 
        return; 
    }

    printf("...\n");
    usleep(500000); 
    printf("⚔️  A WILD MONSTER APPEARS! (HP: %d) ⚔️\n", MONSTER_BASE_HP);
    sleep(1); 

    int monster_hp = MONSTER_BASE_HP;
    
    int player_damage = PLAYER_BASE_ATK; 
    if (p->bagSize > 2 && p->bag[2] > 0) { 
        player_damage = SWORD_ATK_BONUS; 
        printf("(You pull out your Sword! ATK: %d)\n", player_damage);
    } else {
        printf("(You are fighting with bare hands! ATK: %d)\n", player_damage);
    }
    usleep(800000); 

    while (p->hp > 0 && monster_hp > 0) {
        printf("\n----------------------\n");
        printf("[Turn] You attack the monster...\n");
        usleep(500000); 
        
        int actual_dmg = player_damage;
        if (rand() % 100 < 20) {
            actual_dmg *= 2;
            printf("🔥 CRITICAL HIT! 🔥 ");
        }

        printf("You dealt %d damage!\n", actual_dmg);
        monster_hp -= actual_dmg;
        printf("Monster HP: %d\n", monster_hp > 0 ? monster_hp : 0);
        
        if (monster_hp <= 0) {
            usleep(500000);
            printf("\n🏆 VICTORY! You defeated the monster!\n");
            
            int loot_gold = 30 + (rand() % 31); 
            if (rand() % 100 < 5) { 
                printf("✨ RARE DROP! You found a Gem! (+500G)\n");
                p->gold += 500;
            } else {
                printf("Loot: +%d Gold!\n", loot_gold);
                p->gold += loot_gold;
            }
            break; 
        }

        sleep(1); 

        printf("[Turn] The monster attacks you!\n");
        usleep(500000); 

        int monster_atk = MONSTER_BASE_ATK + (rand() % 11 - 5); 
        int hurt = monster_atk;
        int is_blocked = 0; 

        if (p->bagSize > 3 && p->bag[3] > 0) { 
            if (rand() % 100 < SHIELD_BLOCK_CHANCE) {
                is_blocked = 1; 
                hurt = 0;       
                printf("🛡️  BLOCKED! (Your shield negated the damage!)\n");
            }
        }

        if (is_blocked == 0) { 
            if (p->bagSize > 4 && p->bag[4] > 0) { 
                hurt -= ARMOR_DEF_BONUS; 
                if (hurt < 1) hurt = 1; 
                printf("🛡️  CLANG! (Armor blocked %d dmg) ", ARMOR_DEF_BONUS);
            }
        }
        
        p->hp -= hurt;
        if(p->hp < 0) p->hp = 0;
        
        if (hurt > 0) {
             printf("You took %d damage. Your HP: %d\n", hurt, p->hp);
        } else {
             printf("You took 0 damage! Your HP: %d\n", p->hp);
        }

        printf("----------------------\n");
        sleep(1); 
    }

    if (p->hp <= 0) {
        printf("\n☠️ YOU DIED... GAME OVER ☠️\n");
        while(1) {
            printf("Type 'restart' to respawn (-50 Gold penalty), or 'exit' > ");
            scanf("%s", input);

            if (strcmp(input, "restart") == 0) {
                printf(">>> RESPAWNING... <<<\n");
                p->hp = 50; 
                p->gold -= 50; 
                if(p->gold < 0) p->gold = 0;
                break; 
            } else if (strcmp(input, "exit") == 0) {
                printf("Goodbye!\n");
                if (p->bag != NULL) free(p->bag);
                exit(0); 
            } else {
                printf("Invalid command.\n");
            }
        }
    }
    
    printf("------------------------------\n");
    sleep(2);
}

void ShowMenu() {
    printf("--- SHOP MENU ---\n");
    printf("0: Apple       ($10)  [Heal 15 HP]\n");
    printf("1: Bread       ($30)  [Heal 40 HP]\n");
    printf("2: Iron Sword  ($150) [Atk = 30]\n");
    printf("3: Wood Shield ($200) [25%% Block Chance]\n"); 
    printf("4: Iron Armor  ($300) [Def +5]\n");
    printf("5: Mystery Box ($100) [Win $1000?]\n");
    printf("6: Bag Upgrade ($500) [+2 Slots]\n");
    printf("-----------------\n");
    printf("(Cmd: 'id', 'bag', 'eat', 'hunt', 'open', 'save', 'load', 'exit')\n");
}

int main() {
    setbuf(stdout, NULL);
    srand(time(NULL));

    struct Player hero;
    hero.gold = 100;
    hero.hp = 150;
    hero.maxhp = 150;
    hero.bagSize = 6;
    hero.bag = (int*)malloc(hero.bagSize * sizeof(int));

    if (hero.bag == NULL) {
        printf("Fatal Error: Memory allocation failed.\n");
        return 1;
    }
    
    for (int i = 0; i < hero.bagSize; i++) {
        hero.bag[i] = 0;
    }

    struct Item items[7] = {
        {0,"Apple", 10},
        {1,"Bread", 30},
        {2,"Sword", 150},
        {3,"Shield", 200},
        {4,"Armor", 300},
        {5,"Mystery Box", 100},
        {6,"Bag Upgrade", 500}
    };
   
    int id = 0;
    int count = 0;
    char input[50]; 

    printf(">>> Game Start! <<<\n");
    printf("Welcome to the Hardcore Item Shop!\n");
    sleep(1); 

    while(1) {
        system("clear"); 

        Showstatus(hero.gold, hero.hp, hero.maxhp);
        ShowMenu(); 

        printf("Command > ");
        scanf("%s", input); 

        if (strcmp(input, "save") == 0) {
            SaveGame(&hero); 
            continue;
        }

        if (strcmp(input, "load") == 0) {
            LoadGame(&hero);
            continue;
        }

        if (strcmp(input, "bag") == 0) {
            printf("\n--- Your Bag (Size: %d) ---\n", hero.bagSize);
            int empty = 1;
            for (int i = 0; i < hero.bagSize; i++) {
                if (hero.bag[i] > 0) {
                    // 只显示已知的物品名称
                    char* itemName = (i < 7) ? items[i].name : "Unknown Item";
                    printf("[%d] %s: %d\n", i, itemName, hero.bag[i]);
                    empty = 0;
                } 
            }
            if(empty) printf("(Empty)\n");
            printf("---------------\n");

            printf("(Press Enter to continue...)\n");
            getchar(); getchar(); 
            continue; 
        }

        if (strcmp(input, "eat") == 0) {
            printf("\n--- 🍽️  Food Menu 🍽️  ---\n");
            if (hero.bagSize > 0) printf("0: Apple (Heal 15 HP) [Own: %d]\n", hero.bag[0]);
            if (hero.bagSize > 1) printf("1: Bread (Heal 40 HP) [Own: %d]\n", hero.bag[1]);
            printf("-------------------------\n");

            int food_id = -1;
            printf("Eat which item? (Enter ID) > ");
            scanf("%d", &food_id);

            if (food_id != 0 && food_id != 1) {
                printf("You can't eat that!\n");
                sleep(1); continue;
            }

            if (food_id >= hero.bagSize || hero.bag[food_id] <= 0) {
                printf("You don't have any!\n");
                sleep(1); continue;
            }

            if (hero.hp >= hero.maxhp) {
                printf("You are already full HP!\n");
                sleep(1); continue;
            }

            int heal_val = (food_id == 0) ? 15 : 40;
            
            hero.bag[food_id]--;
            hero.hp += heal_val;
            if (hero.hp > hero.maxhp) hero.hp = hero.maxhp; 

            printf("\n>>> Nom nom nom... <<<\n");
            usleep(500000); 
            printf("Recovered %d HP! (Current: %d/%d)\n", heal_val, hero.hp, hero.maxhp);
            sleep(1);
            continue; 
        }

        if (strcmp(input, "hunt") == 0) {
            Battle(&hero); 
            continue; 
        }

        if (strcmp(input, "open") == 0) {
            if (hero.bagSize <= 5 || hero.bag[5] <= 0) {
                printf("No Mystery Boxes!\n");
                sleep(1); continue;
            }

            printf("Opening a Mystery Box... (Cost you $100 to buy)\n");
            hero.bag[5]--; 
            usleep(800000);

            int luck = rand() % 100;
            if (luck < 10) { 
                printf("💎 JACKPOT! You found $%d!\n", JACKPOT_PRIZE);
                hero.gold += JACKPOT_PRIZE;
            } else {
                printf("💨 Poof... It was empty. Better luck next time.\n");
            }
            
            printf("\n(Press Enter...)\n");
            getchar(); getchar();
            continue; 
        }

        if (strcmp(input, "exit") == 0) {
            printf("Goodbye!\n" );
            break;
        }

        id = atoi(input);
        
        if (id == 0 && input[0] != '0') {
             printf("Invalid Command!\n");
             sleep(1);
             continue;
        }

        if (id < 0 || id > 6) {
            printf("Error: Invalid ID!\n");
            sleep(1); 
            continue; 
        }

        printf("You chose [%s] ($%d). How many? > ", items[id].name, items[id].price);
        scanf("%d" , &count);

        if (count <= 0){
            printf("Purchase cancelled.\n");
            sleep(1);
            continue;
        }

        int total = CalTotalPrice(items[id].price , count);

        if (total <= hero.gold) {
            hero.gold -= total;

            if (id == 6) {
                printf("Upgrading backpack...\n");
                sleep(1);
                
                int newSize = hero.bagSize + (2 * count);
                int* temp = (int*)realloc(hero.bag, newSize * sizeof(int));
                
                if (temp != NULL) {
                    hero.bag = temp; 
                    for (int i = hero.bagSize; i < newSize; i++) {
                        hero.bag[i] = 0;
                    }
                    hero.bagSize = newSize; 
                    printf("Success! Bag size increased to %d slots!\n", newSize);
                } else {
                    printf("Error: Memory allocation failed!\n");
                    hero.gold += total; 
                }
            } else {
                if (id >= hero.bagSize) {
                    printf("Your bag is too small for this item! Upgrade it first.\n");
                    hero.gold += total; 
                } else {
                    hero.bag[id] += count;
                    printf("Success! Bought %d %s(s).\n", count, items[id].name);
                }
            }
            sleep(1);
        } else {
            printf("Not enough gold! You need $%d.\n", total);
            sleep(1);
        }
    } 
    
    free(hero.bag);
    return 0;
}