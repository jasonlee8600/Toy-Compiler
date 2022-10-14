
.globl boo
boo:
pushq %rbp
movq %rsp, %rbp
subq $24, %rsp
movq %rdi, -8(%rbp)
movq %rsi, -16(%rbp)
movq -8(%rbp), %rax
addq $12, %rax
movq %rax, -24(%rbp)
movq -24(%rbp), %rax
movq %rbp, %rsp
popq  %rbp
retq

.globl foo
foo:
pushq %rbp
movq %rsp, %rbp
subq $40, %rsp
movq %rdi, -8(%rbp)
movq %rsi, -16(%rbp)
movq -8(%rbp), %rax
addq $12, %rax
movq %rax, -24(%rbp)
movq $24, %rdi
movq -24(%rbp), %rsi
callq boo
movq %rax, -32(%rbp)
movq -32(%rbp), %rax
subq -24(%rbp), %rax
movq %rax, -40(%rbp)
movq -40(%rbp), %rax
movq %rbp, %rsp
popq  %rbp
retq
