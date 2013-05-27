SOPE - Projeto 1 - Backup & Restore
===================================

	O directório src contém o código fonte dos programas
individuais (backup (bckp.c) e restore (rstr.c)) enquanto que o
directório lib contém o código fonte que é partilhado pelos dois
programas.

	O Makefile tem esta estrutura em consideração e criará os
executáveis num directório bin, como pedido.

É necessário pelo menos a versão 4.7.2 do GCC.

------

	Todos os comentários de macros, structs e funções estão em formato
"doxygen". O doxyfile criado por nós para gerar a documentação em html
está disponível em http://paste2.org/twGNM5h2.

------

- "An efficient restore algorithm will be appreciated."

	De forma a tornar a restauração de ficheiros bastante
eficiente, foi guardado em cada __bckpinfo__ o número da iteração
(primeiro backup será a 0, segundo backup será a 1 e assim
sucessivamente) da última modificação ao ficheiro. Desta forma,
apenas lendo o __bckpinfo__ do restauro pretendido é possível
recuperar todos os ficheiros necessários, sem ser preciso
percorrer todos os outros directórios de backup.

------

FEUP - Faculdade de Engenharia da Universidade do Porto

Duarte Duarte - ei11101
Miguel Marques - ei11099
2MIEIC01

21 de Abril de 2013

