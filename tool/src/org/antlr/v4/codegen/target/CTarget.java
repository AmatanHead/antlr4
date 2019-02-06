/*
 * Copyright (c) 2012-2017 The ANTLR Project. All rights reserved.
 * Use of this file is governed by the BSD 3-clause license that
 * can be found in the LICENSE.txt file in the project root.
 */

package org.antlr.v4.codegen.target;

import org.antlr.v4.codegen.CodeGenerator;
import org.antlr.v4.codegen.Target;
import org.antlr.v4.codegen.UnicodeEscapes;
import org.antlr.v4.tool.ErrorType;
import org.antlr.v4.tool.ast.GrammarAST;
import org.stringtemplate.v4.*;
import org.stringtemplate.v4.misc.STMessage;

import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

public class CTarget extends Target {
	private static final Set<String> badWords = new HashSet<>(Arrays.asList(
		"alignas", "alignof", "and", "and_eq", "asm", "auto", "bitand",
		"bitor", "bool", "break", "case", "catch", "char", "char16_t",
		"char32_t", "class", "compl", "concept", "const", "constexpr",
		"const_cast", "continue", "decltype", "default", "delete", "do",
		"double", "dynamic_cast", "else", "enum", "explicit", "export",
		"extern", "false", "float", "for", "friend", "goto", "if",
		"inline", "int", "long", "mutable", "namespace", "new",
		"noexcept", "not", "not_eq", "nullptr", "operator", "or",
		"or_eq", "private", "protected", "public", "register",
		"reinterpret_cast", "requires", "return", "short", "signed",
		"sizeof", "static", "static_assert", "static_cast", "struct",
		"switch", "template", "this", "thread_local", "throw", "true",
		"try", "typedef", "typeid", "typename", "union", "unsigned",
		"using", "virtual", "void", "volatile", "wchar_t", "while",
		"xor", "xor_eq",

		"rule", "parserRule"
	));

	public CTarget(CodeGenerator gen) {
		super(gen, "C");
	}

	@Override
	public String getVersion() {
		return "4.7.2";
	}

	@Override
	public boolean wantsBaseListener() {
		return false;
	}

	@Override
	public boolean wantsBaseVisitor() {
		return false;
	}

	@Override
	public boolean supportsOverloadedMethods() {
		return false;
	}

	@Override
	public boolean needsHeader() {
		return true;
	}

	@Override
	public String encodeIntAsCharEscape(int v) {
		return "0x" + Integer.toHexString(v) + ", ";
	}

	@Override
	protected boolean visibleGrammarSymbolCausesIssueInGeneratedCode(GrammarAST idNode) {
		return badWords.contains(idNode.getText());
	}

	@Override
	protected void appendUnicodeEscapedCodePoint(int codePoint, StringBuilder sb) {
		// C99 and Python share the same escaping style.
		UnicodeEscapes.appendPythonStyleEscapedCodePoint(codePoint, sb);
	}
}
