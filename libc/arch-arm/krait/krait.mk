$(call libc-add-cpu-variant-src,MEMCPY,arch-arm/krait/bionic/memcpy-krait.S)
$(call libc-add-cpu-variant-src,MEMSET,arch-arm/krait/bionic/memset.S)
$(call libc-add-cpu-variant-src,STRCMP,arch-arm/krait/bionic/strcmp.S)

include bionic/libc/arch-arm/generic/generic.mk
