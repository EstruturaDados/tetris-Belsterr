#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// ========== DEFINIÇÕES DE ESTRUTURAS ==========

typedef struct {
    char tipo;          // 'I', 'O', 'T', 'L', 'S', 'Z', 'J'
    int id;
} Peca;

typedef struct {
    Peca peca;
    int timestamp;      // Para desfazer
} Historico;

typedef struct {
    Peca fila[5];
    int inicio;
    int tamanho;
} FilaCircular;

typedef struct {
    Peca pilha[3];
    int topo;
} PilhaReserva;

// ========== VARIÁVEIS GLOBAIS ==========

FilaCircular fila;
PilhaReserva pilha;
Historico historico[20];
int pos_historico = 0;
int contador_pecas = 1;

// ========== CARACTERES DE PEÇAS ==========

const char TIPOS_PECAS[] = {'I', 'O', 'T', 'L', 'S', 'Z', 'J'};
const int NUM_TIPOS = 7;

const char* descricao_peca(char tipo) {
    switch (tipo) {
        case 'I': return "Reta (I)";
        case 'O': return "Quadrado (O)";
        case 'T': return "T";
        case 'L': return "L";
        case 'S': return "S";
        case 'Z': return "Z";
        case 'J': return "J";
        default: return "Desconhecida";
    }
}

// ========== GERAÇÃO DE PEÇAS ==========

Peca gerar_peca() {
    Peca p;
    p.tipo = TIPOS_PECAS[rand() % NUM_TIPOS];
    p.id = contador_pecas++;
    return p;
}

// ========== INICIALIZAÇÃO ==========

void inicializar_fila() {
    fila.inicio = 0;
    fila.tamanho = 0;
    
    printf("🔄 Inicializando fila circular com 5 peças...\n\n");
    
    for (int i = 0; i < 5; i++) {
        Peca p = gerar_peca();
        fila.fila[(fila.inicio + fila.tamanho) % 5] = p;
        fila.tamanho++;
        printf("   ✅ Peça adicionada: %s (ID: %d)\n", descricao_peca(p.tipo), p.id);
    }
}

void inicializar_pilha() {
    pilha.topo = -1;
    printf("\n📚 Pilha de reserva inicializada (capacidade: 3)\n\n");
}

// ========== EXIBIÇÃO ==========

void exibir_fila() {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║        📋 FILA DE PRÓXIMAS PEÇAS       ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    
    printf("┌─────────────────────────────────────┐\n");
    
    if (fila.tamanho == 0) {
        printf("│ ❌ Fila vazia!                      │\n");
    } else {
        for (int i = 0; i < fila.tamanho; i++) {
            int idx = (fila.inicio + i) % 5;
            char* marca = (i == 0) ? "▶️ " : "  ";  // CORRIGIDO: char* em vez de char
            printf("│ %s [%d] %s (ID: %d)\n", 
                   marca,
                   i + 1,
                   descricao_peca(fila.fila[idx].tipo),
                   fila.fila[idx].id);
        }
    }
    
    printf("└─────────────────────────────────────┘\n");
}

void exibir_pilha() {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║      📚 PILHA DE RESERVA (MAX: 3)     ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    
    printf("┌─────────────────────────────────────┐\n");
    
    if (pilha.topo == -1) {
        printf("│ ❌ Pilha vazia!                     │\n");
    } else {
        for (int i = pilha.topo; i >= 0; i--) {
            char* marca = (i == pilha.topo) ? "⭐" : "  ";  // CORRIGIDO: char* em vez de char
            printf("│ %s [%d] %s (ID: %d)\n",
                   marca,
                   i + 1,
                   descricao_peca(pilha.pilha[i].tipo),
                   pilha.pilha[i].id);
        }
    }
    
    printf("└─────────────────────────────────────┘\n");
}

void exibir_estado_completo() {
    printf("\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║    🎮 TETRIS STACK - ESTADO COMPLETO   ║\n");
    printf("╚════════════════════════════════════════╝\n");
    
    exibir_fila();
    exibir_pilha();
}

// ========== OPERAÇÕES COM FILA ==========

void jogar_peca() {
    if (fila.tamanho == 0) {
        printf("\n❌ Fila vazia! Não há peças para jogar.\n");
        return;
    }
    
    Peca p = fila.fila[fila.inicio];
    
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║           ✅ PEÇA JOGADA!             ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    printf("🎮 Você jogou: %s (ID: %d)\n", descricao_peca(p.tipo), p.id);
    
    // Salva no histórico
    historico[pos_historico].peca = p;
    historico[pos_historico].timestamp = 1; // Ação: jogar
    pos_historico++;
    
    // Remove da fila (dequeue)
    fila.inicio = (fila.inicio + 1) % 5;
    fila.tamanho--;
    
    // Adiciona nova peça automaticamente
    Peca nova = gerar_peca();
    fila.fila[(fila.inicio + fila.tamanho) % 5] = nova;
    fila.tamanho++;
    
    printf("🔄 Nova peça adicionada à fila: %s (ID: %d)\n", 
           descricao_peca(nova.tipo), nova.id);
    
    exibir_estado_completo();
}

// ========== OPERAÇÕES COM PILHA ==========

void reservar_peca() {
    if (pilha.topo >= 2) {
        printf("\n❌ Pilha cheia! Máximo de 3 peças em reserva.\n");
        return;
    }
    
    if (fila.tamanho == 0) {
        printf("\n❌ Fila vazia! Não há peças para reservar.\n");
        return;
    }
    
    Peca p = fila.fila[fila.inicio];
    
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║          🔖 PEÇA RESERVADA!           ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    printf("📚 Reservada: %s (ID: %d)\n", descricao_peca(p.tipo), p.id);
    
    // Salva no histórico
    historico[pos_historico].peca = p;
    historico[pos_historico].timestamp = 2; // Ação: reservar
    pos_historico++;
    
    // Push para pilha
    pilha.pilha[++pilha.topo] = p;
    
    // Remove da fila
    fila.inicio = (fila.inicio + 1) % 5;
    fila.tamanho--;
    
    // Adiciona nova peça automaticamente
    Peca nova = gerar_peca();
    fila.fila[(fila.inicio + fila.tamanho) % 5] = nova;
    fila.tamanho++;
    
    printf("🔄 Nova peça adicionada à fila: %s (ID: %d)\n", 
           descricao_peca(nova.tipo), nova.id);
    
    exibir_estado_completo();
}

void usar_peca_reservada() {
    if (pilha.topo == -1) {
        printf("\n❌ Pilha vazia! Não há peças reservadas.\n");
        return;
    }
    
    Peca p = pilha.pilha[pilha.topo];
    
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║          ✅ PEÇA USADA!               ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    printf("🎮 Você usou: %s (ID: %d) da reserva\n", descricao_peca(p.tipo), p.id);
    
    // Salva no histórico
    historico[pos_historico].peca = p;
    historico[pos_historico].timestamp = 3; // Ação: usar
    pos_historico++;
    
    // Pop da pilha
    pilha.topo--;
    
    exibir_estado_completo();
}

// ========== OPERAÇÕES AVANÇADAS ==========

void trocar_peca() {
    if (pilha.topo == -1) {
        printf("\n❌ Pilha vazia! Não há peças para trocar.\n");
        return;
    }
    
    if (fila.tamanho == 0) {
        printf("\n❌ Fila vazia! Não há peças para trocar.\n");
        return;
    }
    
    Peca temp = pilha.pilha[pilha.topo];
    pilha.pilha[pilha.topo] = fila.fila[fila.inicio];
    fila.fila[fila.inicio] = temp;
    
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║        🔄 TROCA REALIZADA!            ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    printf("🔄 Trocadas: %s (reserva) ↔ %s (fila)\n",
           descricao_peca(pilha.pilha[pilha.topo].tipo),
           descricao_peca(fila.fila[fila.inicio].tipo));
    
    // Salva no histórico
    historico[pos_historico].peca = temp;
    historico[pos_historico].timestamp = 4; // Ação: trocar
    pos_historico++;
    
    exibir_estado_completo();
}

void desfazer() {
    if (pos_historico == 0) {
        printf("\n❌ Nenhuma ação anterior para desfazer!\n");
        return;
    }
    
    pos_historico--;
    Peca p = historico[pos_historico].peca;
    int acao = historico[pos_historico].timestamp;
    
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║       ⏮️  AÇÃO DESFEITA!              ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    
    switch (acao) {
        case 1:
            printf("⏮️  Peça jogada foi recolocada: %s (ID: %d)\n",
                   descricao_peca(p.tipo), p.id);
            break;
        case 2:
            printf("⏮️  Peça removida da reserva: %s (ID: %d)\n",
                   descricao_peca(p.tipo), p.id);
            break;
        case 3:
            printf("⏮️  Peça retornou à reserva: %s (ID: %d)\n",
                   descricao_peca(p.tipo), p.id);
            break;
        case 4:
            printf("⏮️  Troca desfeita\n");
            break;
    }
    
    exibir_estado_completo();
}

void inverter_estruturas() {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║      🔀 INVERSÃO DE ESTRUTURAS!       ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    
    printf("⚠️  Esta é uma operação experimental!\n");
    printf("   Você está invertendo a fila com a pilha...\n\n");
    
    // Cria backup
    FilaCircular fila_temp = fila;
    PilhaReserva pilha_temp = pilha;
    
    // Limpa as estruturas
    fila.tamanho = 0;
    pilha.topo = -1;
    
    // Adiciona peças da pilha à fila
    for (int i = pilha_temp.topo; i >= 0; i--) {
        fila.fila[fila.tamanho] = pilha_temp.pilha[i];
        fila.tamanho++;
    }
    
    // Completa a fila com as antigas
    for (int i = 0; i < fila_temp.tamanho && fila.tamanho < 5; i++) {
        int idx = (fila_temp.inicio + i) % 5;
        fila.fila[fila.tamanho] = fila_temp.fila[idx];
        fila.tamanho++;
    }
    
    printf("✅ Estruturas invertidas com sucesso!\n");
    
    // Salva no histórico
    historico[pos_historico].peca = fila_temp.fila[fila_temp.inicio];
    historico[pos_historico].timestamp = 6; // Ação: inverter
    pos_historico++;
    
    exibir_estado_completo();
}

// ========== MENU ==========

void exibir_menu() {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║   🎮 TETRIS STACK 🎮                     ║\n");
    printf("╚══════════════════════════════════════════╝\n\n");
    printf("┌────────────────────────────────────────┐\n");
    printf("│ [1] 🎮 Jogar peça                     │\n");
    printf("│ [2] 📚 Reservar peça                  │\n");
    printf("│ [3] ⭐ Usar peça reservada            │\n");
    printf("│ [4] 🔄 Trocar peça (fila ↔ pilha)    │\n");
    printf("│ [5] ⏮️  Desfazer última ação          │\n");
    printf("│ [6] 🔀 Inverter estruturas            │\n");
    printf("│ [0] 🚪 Sair do jogo                  │\n");
    printf("└────────────────────────────────────────┘\n");
    printf("Escolha: ");
}

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pausa() {
    printf("\n⏸️  Pressione ENTER para continuar...");
    getchar();
    printf("\n");
}

void limpar_tela() {
    system("clear || cls");
}

// ========== MAIN ==========

int main() {
    srand(time(NULL));
    int opcao;
    
    // Banner inicial
    limpar_tela();
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║       🎮 TETRIS STACK 🎮                                   ║\n");
    printf("║   Integração Total com Fila Circular e Pilha Linear        ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    printf("🎯 Bem-vindo ao desafio final!\n");
    printf("   Você irá controlar um sistema avançado de peças Tetris\n");
    printf("   com operações estratégicas e desfazer.\n\n");
    
    // Inicializa estruturas
    inicializar_fila();
    inicializar_pilha();
    
    exibir_estado_completo();
    pausa();
    
    // Loop do menu
    while (1) {
        limpar_tela();
        exibir_menu();
        scanf("%d", &opcao);
        limpar_buffer();
        
        limpar_tela();
        
        switch (opcao) {
            case 1:
                jogar_peca();
                break;
            case 2:
                reservar_peca();
                break;
            case 3:
                usar_peca_reservada();
                break;
            case 4:
                trocar_peca();
                break;
            case 5:
                desfazer();
                break;
            case 6:
                inverter_estruturas();
                break;
            case 0:
                printf("\n╔════════════════════════════════════════════════════════════╗\n");
                printf("║    👋 Parabéns por completar o TETRIS STACK 👋               ║\n");
                printf("║          Você dominou filas e pilhas com maestria!           ║\n");
                printf("╚══════════════════════════════════════════════════════════════╝\n\n");
                return 0;
            default:
                printf("❌ Opção inválida! Tente novamente.\n");
        }
        
        if (opcao != 0) {
            pausa();
        }
    }
    
    return 0;
}
