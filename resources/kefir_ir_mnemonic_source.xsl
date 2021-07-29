<?xml version="1.0"?>
<xsl:stylesheet version="1.1"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text" encoding="utf-8" media-type="text/plain" />
    <xsl:variable name="lcletters">abcdefghijklmnopqrstuvwxyz</xsl:variable>
    <xsl:variable name="ucletters">ABCDEFGHIJKLMNOPQRSTUVWXYZ</xsl:variable>
	<xsl:template match="opcodes">
		<xsl:text>/*
    SPDX-License-Identifier: GPL-3.0

    Copyright (C) 2020-2021  Jevgenijs Protopopovs

    This file is part of Kefir project.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see &lt;http://www.gnu.org/licenses/&gt;.
*/

#include "kefir/ir/mnemonic.h"
#include "kefir/core/basic-types.h"

// Do not edit this file. It is autogenerated. Edit resources/opcodes.xml instead.
static struct {
    kefir_iropcode_t opcode;
    const char *symbol;
} OPCODE_SYMBOLS[] = {
</xsl:text>
		<xsl:for-each select="opcode">
			<xsl:text>    { KEFIR_IROPCODE_</xsl:text>
			<xsl:value-of select="translate(@id, $lcletters, $ucletters)" />
			<xsl:text>, KEFIR_IROPCODE_MNEMONIC_</xsl:text>
			<xsl:value-of select="translate(@id, $lcletters, $ucletters)" />
			<xsl:text>}, &#xa;</xsl:text>
		</xsl:for-each>
		<xsl:text>};

const char *kefir_iropcode_mnemonic(kefir_iropcode_t opcode) {
    for (kefir_size_t i = 0; i &lt; sizeof(OPCODE_SYMBOLS) / sizeof(OPCODE_SYMBOLS[0]); i++) {
        if (OPCODE_SYMBOLS[i].opcode == opcode) {
            return OPCODE_SYMBOLS[i].symbol;
        }
    }
    return NULL;
}&#xa;</xsl:text>
	</xsl:template>

</xsl:stylesheet>