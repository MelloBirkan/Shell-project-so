#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define IDNAME 30 // Tamanho maximo de um nome de diretório ou arquivo
#define PATH 100  // Tamanho maximo de um caminho

long obterTamanhoArquivo(const char *nomeArquivo);

// Estrutura para os vértices ou seja os diretórios
typedef struct _TNo {
  char id[IDNAME + 1];  // Nome do diretório ou arquivo
  char path[PATH + 1];  // Caminho a partir da raiz até o ID
  char tipo;            // Se é Arquivo ('a') ou Diretório ('d')
  long long tamanho;    // Tamanho do arquivo, se for um arquivo
  struct _TNo *dir_ant; // Diretório anterior
  struct _TNo *filhos;  // Lista de diretórios ou arquivos a partir do corrente
  struct _TNo *prox;    // Lista de diretórios ou arquivos a partir do corrente
} TVertice;

// Estrutura para controlar a função percorrer a largura
typedef struct _TFila {
  TVertice *no;
  struct _TFila *prox;
} TFila;

// funcao para criar um novo no (diretorio ou arquivo)
TVertice *criarNo(char *id, char *path, char tipo) {
  TVertice *novoNo = (TVertice *)malloc(sizeof(TVertice));
  strcpy(novoNo->id, id);
  strcpy(novoNo->path, path);
  novoNo->tipo = tipo;
  novoNo->dir_ant = NULL;
  novoNo->filhos = NULL;
  novoNo->prox = NULL;
  return novoNo;
}

// Função para inserir um novo no na Arvore
void inserirNo(TVertice *diretorioAtual, TVertice *novoNo) {
  novoNo->dir_ant = diretorioAtual;
  if (diretorioAtual->filhos == NULL) {
    diretorioAtual->filhos = novoNo;
  } else {
    TVertice *ultimoNo = diretorioAtual->filhos;
    while (ultimoNo->prox != NULL) {
      ultimoNo = ultimoNo->prox;
    }
    ultimoNo->prox = novoNo;
  }
}

// função para remover um no da Arvore
void removerNo(TVertice *diretorioAtual, char *id) {
  TVertice *noAnterior = NULL;
  TVertice *noRemover = diretorioAtual->filhos;

  while (noRemover != NULL && strcmp(noRemover->id, id) != 0) {
    noAnterior = noRemover;
    noRemover = noRemover->prox;
  }

  if (noRemover == NULL) {
    printf("Erro: O arquivo ou direto '%s' no foi encontrado.\n", id);
    return;
  }

  if (noAnterior == NULL) {
    diretorioAtual->filhos = noRemover->prox;
  } else {
    noAnterior->prox = noRemover->prox;
  }

  free(noRemover);
}

// funçao para obter o tamanho do arquivo
long obterTamanhoArquivo(const char *nomeArquivo) {
  struct stat st;
  if (stat(nomeArquivo, &st) == 0) {
    return st.st_size;
  } else {
    // Erro ao obter o tamanho do arquivo
    return -1;
  }
}

void percorrerProfundidade(TVertice *no) {
  printf("%s\n", no->path);

  if (no->tipo == 'a') {
    struct stat st;
    if (stat(no->path, &st) == 0) {
      printf("Tamanho: %lld bytes\n", st.st_size);
    } else {
      printf("Erro ao obter o tamanho do arquivo.\n");
    }
  }

  TVertice *filho = no->filhos;
  while (filho != NULL) {
    percorrerProfundidade(filho);
    filho = filho->prox;
  }
}

// funçao para percorrer em largura
void percorrerLargura(TVertice *raiz) {
  TFila *fila = NULL;
  TFila *fimFila = NULL;

  fila = (TFila *)malloc(sizeof(TFila));
  fila->no = raiz;
  fila->prox = NULL;
  fimFila = fila;

  while (fila != NULL) {
    TVertice *no = fila->no;
    printf("%s\n", no->path);

    if (no->tipo == 'a') {
      // printf("Tamanho: <obter tamanho do arquivo>\n");
      struct stat st;
      if (stat(no->path, &st) == 0) {
        printf("Tamanho: %lld bytes\n", st.st_size);
      } else {
        printf("Erro ao obter o tamanho do arquivo.\n");
      }
    }

    TVertice *filho = no->filhos;
    while (filho != NULL) {
      TFila *novoNoFila = (TFila *)malloc(sizeof(TFila));
      novoNoFila->no = filho;
      novoNoFila->prox = NULL;

      if (fila->prox == NULL) {
        fila->prox = novoNoFila;
        fimFila = novoNoFila;
      } else {
        fimFila->prox = novoNoFila;
        fimFila = novoNoFila;
      }

      filho = filho->prox;
    }

    TFila *noRemover = fila;
    fila = fila->prox;
    free(noRemover);
  }
}

// atual
TVertice *encontrarDiretorio(TVertice *diretorioAtual, const char *path) {
  if (strcmp(diretorioAtual->path, path) == 0) {
    return diretorioAtual;
  }

  TVertice *filho = diretorioAtual->filhos;
  while (filho != NULL) {
    TVertice *resultado = encontrarDiretorio(filho, path);
    if (resultado != NULL) {
      return resultado;
    }
    filho = filho->prox;
  }

  return NULL;
}

TVertice *criarDiretorioNome(TVertice *diretorioAtual,
                             const char *nomeDiretorio) {
  TVertice *filho = diretorioAtual->filhos;
  while (filho != NULL) {
    if (strcmp(filho->id, nomeDiretorio) == 0) {
      printf("Erro:Diretorio j· existe.\n");
      return NULL;
    }
    filho = filho->prox;
  }

  char novoPath[PATH + 1];
  sprintf(novoPath, "%s/%s", diretorioAtual->path, nomeDiretorio);

  // Usando a funcao mkdir para criar o diretorio
  if (mkdir(novoPath, 0700) != 0) {
    printf("Erro ao criar diretorio.\n");
    return NULL;
  }

  // Criar o novo no para o diretorio
  TVertice *novoDiretorio = criarNo(nomeDiretorio, novoPath, 'd');

  // Adicionar o novo diretorio ‡ lista de filhos do diretorio atual
  if (diretorioAtual->filhos == NULL) {
    diretorioAtual->filhos = novoDiretorio;
  } else {
    TVertice *ultimoFilho = diretorioAtual->filhos;
    while (ultimoFilho->prox != NULL) {
      ultimoFilho = ultimoFilho->prox;
    }
    ultimoFilho->prox = novoDiretorio;
  }

  novoDiretorio->dir_ant = diretorioAtual;

  printf("Diretorio criado com sucesso: %s\n", novoDiretorio->path);

  return novoDiretorio;
}

void carregarDiretorio(TVertice *diretorioAtual) {
  DIR *dir = opendir(diretorioAtual->path);
  if (dir == NULL) {
    printf("Falha ao abrir o diretório '%s'.\n", diretorioAtual->path);
    return;
  }

  struct dirent *ent;
  while ((ent = readdir(dir)) != NULL) {
    // Ignorar os diretórios '.' e '..'
    if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
      continue;
    }

    // Criar o caminho completo para o arquivo/diretório
    char novoPath[PATH + 1];
    sprintf(novoPath, "%s/%s", diretorioAtual->path, ent->d_name);

    if (ent->d_type == DT_DIR) {
      // É um diretório
      TVertice *novoDiretorio = criarNo(ent->d_name, novoPath, 'd');
      inserirNo(diretorioAtual, novoDiretorio);
    } else if (ent->d_type == DT_REG) {
      // É um arquivo
      TVertice *novoArquivo = criarNo(ent->d_name, novoPath, 'a');
      inserirNo(diretorioAtual, novoArquivo);
    }
  }

  closedir(dir);
}

void iniciarCriacaoDiretorio(TVertice *raiz, TVertice *diretorioAtual) {
  char nomeDiretorio[IDNAME + 1];
  printf("Digite o nome do diretorio a ser criado: ");
  scanf("%s", nomeDiretorio);

  TVertice *novoDiretorio = criarDiretorioNome(diretorioAtual, nomeDiretorio);
  if (novoDiretorio == NULL) {
    printf("Falha ao criar o diretorio.\n");
  }
}

TVertice *mudarDiretorio(TVertice *diretorioAtual, const char *nomeDiretorio);

// funçao para mudar de diretorio
//--------------------------------------------------------
TVertice *diretorioAtual;  // Diretorio atual
TVertice *diretorioOrigem; // Diretorio de origem
//--------------------------------------------------------

TVertice *mudarDiretorioCorrente(TVertice *diretorioAtual,
                                 TVertice *diretorioOrigem) {
  char nomeDiretorio[IDNAME + 1];
  printf("Digite o nome do diretorio para acessar: ");
  getchar();
  fgets(nomeDiretorio, sizeof(nomeDiretorio), stdin);
  nomeDiretorio[strcspn(nomeDiretorio, "\n")] = '\0';

  // Verificar se o usu·rio deseja voltar para o diretorio de origem
  if (strcmp(nomeDiretorio, "raiz") == 0) {
    diretorioAtual = diretorioOrigem;
    printf("Diretorio atual: %s\n", diretorioAtual->path);
    return diretorioAtual;
  }

  return mudarDiretorio(diretorioAtual, nomeDiretorio);
}

TVertice *mudarDiretorio(TVertice *carregarDiretorio,
                         const char *nomeDiretorio) {
  // Encontrar o diretorio solicitado
  TVertice *filho = diretorioAtual->filhos;

  while (filho != NULL) {

    if (strcmp(filho->id, nomeDiretorio) == 0) {
      printf("Diretorio encontrado: %s\n", filho->path);
      return filho;
    }
    filho = filho->prox;
  }

  printf("Erro:Diretorio '%s' n„o encontrado.\n", nomeDiretorio);
  return diretorioAtual;
}

void criarArquivo(TVertice *diretorioAtual, const char *nomeArquivo) {
  // Verificar se o arquivo j· existe
  TVertice *filho = diretorioAtual->filhos;
  while (filho != NULL) {
    if (strcmp(filho->id, nomeArquivo) == 0) {
      printf("Erro: Arquivo j· existe.\n");
      return;
    }
    filho = filho->prox;
  }

  // Criar o caminho completo para o arquivo
  char novoPath[PATH + 1];
  sprintf(novoPath, "%s/%s", diretorioAtual->path, nomeArquivo);

  // Usando a funcao fopen para criar o arquivo
  FILE *novoArquivo = fopen(novoPath, "w");
  if (novoArquivo == NULL) {
    printf("Erro ao criar arquivo.\n");
    return;
  }
  fclose(novoArquivo);

  // Criar o novo no para o arquivo
  TVertice *novoArquivoNo = criarNo(nomeArquivo, novoPath, 'a');

  // Adicionar o novo arquivo ‡ lista de filhos do diretorio atual
  inserirNo(diretorioAtual, novoArquivoNo);

  printf("Arquivo criado com sucesso: %s\n", novoArquivoNo->path);
}

// funcao para remover um arquivo do diretorio atual
void removerArquivo(TVertice *diretorioAtual, const char *nomeArquivo) {
  // Verificar se o arquivo existe
  TVertice *filho = diretorioAtual->filhos;
  TVertice *noAnterior = NULL;
  TVertice *noRemover = NULL;

  while (filho != NULL) {
    if (strcmp(filho->id, nomeArquivo) == 0) {
      noRemover = filho;
      break;
    }
    noAnterior = filho;
    filho = filho->prox;
  }

  if (noRemover == NULL) {
    printf("Erro: Arquivo '%s' não encontrado.\n", nomeArquivo);
    return;
  }

  if (noAnterior == NULL) {
    diretorioAtual->filhos = noRemover->prox;
  } else {
    noAnterior->prox = noRemover->prox;
  }

  // Remover o arquivo do sistema de arquivos
  if (remove(noRemover->path) != 0) {
    printf("Erro ao remover o arquivo '%s'.\n", nomeArquivo);
    return;
  }

  free(noRemover);
  printf("Arquivo removido com sucesso: %s\n", nomeArquivo);
}

// funcao para liberar a memoria da estrutura de diretorios e arquivos
void liberarMemoria(TVertice *diretorio) {
  if (diretorio == NULL) {
    return;
  }

  // Liberar os nos filhos recursivamente
  TVertice *filho = diretorio->filhos;
  while (filho != NULL) {
    TVertice *proximo = filho->prox;
    liberarMemoria(filho);
    filho = proximo;
  }

  // Liberar o proprio no
  free(diretorio);
}

void criarArquivo(TVertice *diretorioAtual, const char *nomeArquivo);

int main(int argc, char *argv[]) {

  if (argc < 2) {
    printf("ERROR: Abra o arquivo com um path/diretório ao lado do nome.\n");
    return 1;
  }

  // Criar a raiz da Arvore de diretorios
  TVertice *raiz = criarNo("", argv[1], 'd');

  TVertice *diretorioAtual = raiz;

  TVertice *diretorioOrigem = diretorioAtual;

  char comando;
  carregarDiretorio(diretorioAtual);
  do {
    printf("\nComandos disponíveis:\n");
    printf("(a) criar arquivo\n");
    printf("(m) - criar diretório\n");
    printf("(c) - mudar diretório atual\n");
    printf("(d) - remover um arquivo\n");
    printf("(p) - Apresentar a árvore realizando um percurso em profundidade\n");
    printf("(l) - Apresentar a árvore realizando um percurso em largura\n");
    printf("(s) - sair\n");
    printf("Digite o comando: ");

    scanf(" %c", &comando);

    switch (comando) {

    case 'a': {
      // Acrescentar um arquivo ao diretorio atual
      char nomeArquivo[IDNAME + 1];
      printf("Digite o nome do arquivo a ser inserido: ");
      scanf("%s", nomeArquivo);
      criarArquivo(diretorioAtual, nomeArquivo);
    } break;

    case 'm':
      // criarDiretorio(raiz, diretorioAtual);
      iniciarCriacaoDiretorio(raiz, diretorioAtual);

      break;
    case 'c':
      diretorioAtual = mudarDiretorioCorrente(diretorioAtual, diretorioOrigem);
      // diretorioAtual = mudarDiretorioCorrente(diretorioAtual, raiz);
      printf("Diretorio corrente alterado para: %s\n", diretorioAtual->path);
      break;
    case 'd': {
      // Remover um arquivo do diretorio atual
      char nomeArquivoRemover[IDNAME + 1];
      printf("Digite o nome do arquivo a ser removido: ");
      scanf("%s", nomeArquivoRemover);
      removerArquivo(diretorioAtual, nomeArquivoRemover);
    }

    break;
    case 'p':
      percorrerProfundidade(diretorioAtual);
      break;
    case 'l':
      percorrerLargura(raiz);
      break;
    case 's':
      printf("Encerrando o programa.\n");
      break;
    default:
      printf("Comando inv·lido. Tente novamente.\n");
      break;
    }
  } while (comando != 's');

  // Liberar a memoria alocada para a Arvore de diretorios
  liberarMemoria(raiz);

  return 0;
}
