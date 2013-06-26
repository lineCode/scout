/*
 * ###########################################################################
 * Copyright (c) 2010, Los Alamos National Security, LLC.
 * All rights reserved.
 *
 *  Copyright 2010. Los Alamos National Security, LLC. This software was
 *  produced under U.S. Government contract DE-AC52-06NA25396 for Los
 *  Alamos National Laboratory (LANL), which is operated by Los Alamos
 *  National Security, LLC for the U.S. Department of Energy. The
 *  U.S. Government has rights to use, reproduce, and distribute this
 *  software.  NEITHER THE GOVERNMENT NOR LOS ALAMOS NATIONAL SECURITY,
 *  LLC MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY
 *  FOR THE USE OF THIS SOFTWARE.  If software is modified to produce
 *  derivative works, such modified software should be clearly marked,
 *  so as not to confuse it with the version available from LANL.
 *
 *  Additionally, redistribution and use in source and binary forms,
 *  with or without modification, are permitted provided that the
 *  following conditions are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *
 *    * Neither the name of Los Alamos National Security, LLC, Los
 *      Alamos National Laboratory, LANL, the U.S. Government, nor the
 *      names of its contributors may be used to endorse or promote
 *      products derived from this software without specific prior
 *      written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND
 *  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS NATIONAL SECURITY, LLC OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 *  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 *  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 * ###########################################################################
 *
 * Notes
 *
 * #####
 */

#include "clang/Parse/Parser.h"
#include "RAIIObjectsForParser.h"
#include "clang/Basic/AddressSpaces.h"
#include "clang/Basic/CharInfo.h"
#include "clang/Basic/OpenCL.h"
#include "clang/Parse/ParseDiagnostic.h"
#include "clang/Sema/Lookup.h"
#include "clang/Sema/ParsedTemplate.h"
#include "clang/Sema/PrettyDeclStackTrace.h"
#include "clang/Sema/Scope.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringSwitch.h"

// SC_TODO : would be nice to make sure we document the details of why we
// are using a map here... (PM)
#include <map>

using namespace clang;

// scout - Get value of LookAheadToken.  A hack just to
//                 to get value between 2 and 4.  Needs replacement
static int GetValueLAT(const char *v) {
   if ((*v > '4') || (*v < '2')) return 0;
   if ((*(v+1) >= '0') && (*(v+1) <= '9')) return 0;
  return ((int) (*v - '0'));
}

// scout - detect float <4>, int <3>, ... iSPC constructs and
//                 return TST_value of Scout float4, int3, ...
//                 that should be substituted.  Return same if not iSPC
//                 Involves lookahead and possible token consumption
DeclSpec::TST Parser::GetIspcScoutExtension(DeclSpec::TST TagType, tok::TokenKind kw) {


  switch (kw) {
    default: break;
    case tok::kw_bool:
    case tok::kw_char:
    case tok::kw_short:
    case tok::kw_int:
    case tok::kw_long:
    case tok::kw_float:
    case tok::kw_double:
      if ((GetLookAheadToken(1).is(tok::less))             &&
          (GetLookAheadToken(2).is(tok::numeric_constant)) &&
          (GetLookAheadToken(3).is(tok::greater)))            {
        const char *v = GetLookAheadToken(2).getLiteralData();
        int val = GetValueLAT(v);

        if (val < 2 || val > 4) break;
        switch(kw) {
          default: assert(0 && "GetIspc bad kw 2 logic"); break;
          case tok:: kw_bool:
            switch (val) {
              case 2: TagType = TST_bool2; break;
              case 3: TagType = TST_bool3; break;
              case 4: TagType = TST_bool4; break;
              default: assert(0 && "GetIspc bad bool logic"); break;
            } // end kw_bool
            break;
          case tok:: kw_char:
            switch (val) {
              case 2: TagType = TST_char2; break;
              case 3: TagType = TST_char3; break;
              case 4: TagType = TST_char4; break;
              default: assert(0 && "GetIspc bad char logic"); break;
            } // end kw_char
            break;
          case tok:: kw_short:
            switch (val) {
              case 2: TagType = TST_short2; break;
              case 3: TagType = TST_short3; break;
              case 4: TagType = TST_short4; break;
              default: assert(0 && "GetIspc bad short logic"); break;
            } // end kw_short
            break;
          case tok:: kw_int:
            switch (val) {
              case 2: TagType = TST_int2; break;
              case 3: TagType = TST_int3; break;
              case 4: TagType = TST_int4; break;
              default: assert(0 && "GetIspc bad int logic"); break;
            } // end kw_int
            break;
          case tok:: kw_long:
            switch (val) {
              case 2: TagType = TST_long2; break;
              case 3: TagType = TST_long3; break;
              case 4: TagType = TST_long4; break;
              default: assert(0 && "GetIspc bad long logic"); break;
            } // end kw_long
            break;
          case tok:: kw_float:
            switch (val) {
              case 2: TagType = TST_float2; break;
              case 3: TagType = TST_float3; break;
              case 4: TagType = TST_float4; break;
              default: assert(0 && "GetIspc bad float logic"); break;
            } // end kw_float
            break;
          case tok:: kw_double:
            switch (val) {
              case 2: TagType = TST_double2; break;
              case 3: TagType = TST_double3; break;
              case 4: TagType = TST_double4; break;
              default: assert(0 && "GetIspc bad double logic"); break;
            } // end kw_double
            break;
          } // end switch(kw)
        } // end if lookahead
        break; // endcase of all scout kw_bool2 ... kw_double4
  } // endswitch(kw)

  return TagType;
}
// scout - Scout Mesh
// this method is used to parse a mesh field declaration, e.g:
// uniform mesh MyMesh[512,512]{
//   cells:
//     float a; // <---- parser is here
// ... }

void Parser::
ParseMeshDeclaration(ParsingDeclSpec &DS,
                     FieldCallback &Fields,
                     unsigned FieldType) {
  ParseSpecifierQualifierList(DS);

  // Read mesh-declarators until we find the semicolon.
  bool FirstDeclarator = true;
  while (1) {
    ParsingDeclRAIIObject PD(*this, ParsingDeclRAIIObject::NoParent);
    ParsingFieldDeclarator DeclaratorInfo(*this, DS);

    // Attributes are only allowed here on successive declarators.
    if (!FirstDeclarator)
      MaybeParseGNUAttributes(DeclaratorInfo.D);

    ParseDeclarator(DeclaratorInfo.D);

    // If attributes exist after the declarator, parse them.
    MaybeParseGNUAttributes(DeclaratorInfo.D);

    // We're done with this declarator;  invoke the callback.
    Fields.invoke(DeclaratorInfo);

    // If we don't have a comma, it is either the end of the list (a ';')
    // or an error, bail out.
    if (Tok.isNot(tok::comma)){
      return;
    }

    // Consume the comma.
    ConsumeToken();

    FirstDeclarator = false;
  }
}
// scout - parse a mesh parameter declaration
// assumes on entry that the token stream looks like:
// [], [:], [::], and that we have already parsed a mesh type
void Parser::ParseMeshParameterDeclaration(DeclSpec& DS){
  ParsedType parsedType = DS.getRepAsType();
  const UniformMeshType* mt = dyn_cast<UniformMeshType>(parsedType.get().getTypePtr());

  ConsumeBracket();
  size_t numDims;
  if(Tok.is(tok::r_square)){
    numDims = 1;
  }
  else if(Tok.is(tok::colon)){
    numDims = 2;
    ConsumeToken();
  }
  else if(Tok.is(tok::coloncolon)){
    numDims = 3;
    ConsumeToken();
  }
  else{
    Diag(Tok, diag::err_expected_mesh_param_token);
    SkipUntil(tok::r_square);
    return;
  }

  if(Tok.isNot(tok::r_square)){
    Diag(Tok, diag::err_expected_mesh_param_token);
    SkipUntil(tok::r_square);
    return;
  }
  else{
    ConsumeBracket();
  }

  MeshType::MeshDimensionVec dims;
  for(size_t i = 0; i < numDims; ++i){
    dims.push_back(Actions.ActOnIntegerConstant(Tok.getLocation(), 0).get());
  }

  UniformMeshType* mdt = new UniformMeshType(mt->getDecl()); //SC_TODO: possible alignment problem?

  mdt->setDimensions(dims);
  parsedType.set(QualType(mdt, 0));
  DS.UpdateTypeRep(parsedType);
}

// scout - parse a window or image declaration
// return true on success
// these look like:

// window win[1024,1024] {
//   background  = hsv(0.1, 0.2, 0.3);
//   save_frames = true;
//   filename    = "heat2d-####.png";
// };

// image img[1024, 1024]{
//   background = hsv(0.0f, 0.0f, 0.0f);
//   filename   = "heat2d-####.png";
//  };


StmtResult
Parser::ParseWindowOrImageDeclaration(bool window,
                                      StmtVector &Stmts,
                                      bool OnlyStatement){
  if(window){
    assert(Tok.is(tok::kw_window) && "Not a window declaration stmt!");
  }
  else{
    assert(Tok.is(tok::kw_image) && "Not an image declaration stmt!");
  }

  ConsumeToken();

  if(Tok.isNot(tok::identifier)){
    Diag(Tok, diag::err_expected_ident);
    SkipUntil(tok::semi);
    ConsumeToken();
    return StmtError();
  }

  IdentifierInfo* Name = Tok.getIdentifierInfo();
  SourceLocation NameLoc = ConsumeToken();

  if(Tok.isNot(tok::l_square)){
    Diag(Tok, diag::err_expected_lsquare);

    SkipUntil(tok::r_brace);
    SkipUntil(tok::semi);
    ConsumeToken();
    return StmtError();
  }

  ConsumeBracket();

  if(Tok.isNot(tok::numeric_constant)){
    Diag(Tok, diag::err_expected_numeric_constant_in_window_def);

    SkipUntil(tok::r_brace);
    SkipUntil(tok::semi);
    ConsumeToken();
    return StmtError();
  }

  ExprResult XSize = Actions.ActOnNumericConstant(Tok).get();

  ConsumeToken();

  if(Tok.isNot(tok::comma)){
    Diag(Tok, diag::err_expected_comma);

    SkipUntil(tok::r_brace);
    SkipUntil(tok::semi);
    ConsumeToken();
    return StmtError();
  }

  ConsumeToken();

  ExprResult YSize = Actions.ActOnNumericConstant(Tok).get();

  ConsumeToken();

  if(Tok.isNot(tok::r_square)){
    Diag(Tok, diag::err_expected_rsquare);

    SkipUntil(tok::r_brace);
    SkipUntil(tok::semi);
    ConsumeToken();
    return StmtError();
  }

  ConsumeBracket();

  if(Tok.isNot(tok::l_brace)){
    Diag(Tok, diag::err_expected_lbrace);

    SkipUntil(tok::r_brace);
    SkipUntil(tok::semi);
    ConsumeToken();
    return StmtError();
  }

  ConsumeBrace();

  typedef std::map<std::string, Expr*> ArgExprMap;

  ArgExprMap argExprMap;

  bool error = false;

  for(;;){
    if(Tok.is(tok::r_brace) || Tok.is(tok::eof)){
      break;
    }

    if(Tok.isNot(tok::identifier)){
      Diag(Tok, diag::err_expected_ident);

      SkipUntil(tok::r_brace);
      SkipUntil(tok::semi);
      ConsumeToken();
      return StmtError();
    }

    IdentifierInfo* Arg = Tok.getIdentifierInfo();
    SourceLocation ArgLoc = ConsumeToken();
    (void)ArgLoc; //suppress warning

    if(Tok.isNot(tok::equal)){
      Diag(Tok, diag::err_expected_equal_after) << Arg->getName();

      SkipUntil(tok::r_brace);
      SkipUntil(tok::semi);
      ConsumeToken();
      return StmtError();
    }

    ConsumeToken();

    ExprResult argResult = ParseExpression();
    if(argResult.isInvalid()){
      error = true;
    }

    argExprMap[Arg->getName()] = argResult.get();

    if(Tok.isNot(tok::semi)){
      if(window){
        Diag(Tok, diag::err_expected_semi_window_arg);
      }
      else{
        Diag(Tok, diag::err_expected_semi_image_arg);
      }

      SkipUntil(tok::r_brace);
      SkipUntil(tok::semi);
      ConsumeToken();
      return StmtError();
    }

    ConsumeToken();
  }

  assert(Tok.is(tok::r_brace) && "expected r_brace");

  ConsumeBrace();

  assert(Tok.is(tok::semi) && "expected semi");

  ConsumeToken();

  if(error){
    return StmtError();
  }

  std::string code;

  if(window){
    code = "scout::window_rt " + Name->getName().str() + "(" +
    ToCPPCode(XSize.get()) + ", " + ToCPPCode(YSize.get()) + ", ";

    ArgExprMap::iterator itr = argExprMap.find("background");
    if(itr == argExprMap.end()){
      Diag(Tok, diag::err_missing_field_window_decl) << "background";
      error = true;
    }

    code += ToCPPCode(itr->second) + ".x, ";
    code += ToCPPCode(itr->second) + ".y, ";
    code += ToCPPCode(itr->second) + ".z, ";
    code += ToCPPCode(itr->second) + ".w, ";

    itr = argExprMap.find("save_frames");
    if(itr == argExprMap.end()){
      Diag(Tok, diag::err_missing_field_window_decl) << "save_frames";
      error = true;
    }

    code += ToCPPCode(itr->second) + ", ";

    itr = argExprMap.find("filename");
    if(itr == argExprMap.end()){
      Diag(Tok, diag::err_missing_field_window_decl) << "filename";
      error = true;
    }

    code += ToCPPCode(itr->second) + ");";
  }
  else{
    code = "scout::image_rt " + Name->getName().str() + "(" +
    ToCPPCode(XSize.get()) + ", " + ToCPPCode(YSize.get()) + ", ";

    ArgExprMap::iterator itr = argExprMap.find("background");
    if(itr == argExprMap.end()){
      Diag(Tok, diag::err_missing_field_image_decl) << "background";
      error = true;
    }

    code += ToCPPCode(itr->second) + ".x, ";
    code += ToCPPCode(itr->second) + ".y, ";
    code += ToCPPCode(itr->second) + ".z, ";
    code += ToCPPCode(itr->second) + ".w, ";

    itr = argExprMap.find("filename");
    if(itr == argExprMap.end()){
      Diag(Tok, diag::err_missing_field_image_decl) << "filename";
      error = true;
    }

    code += ToCPPCode(itr->second) + ");";
  }

  if(error){
    return StmtError();
  }

  InsertCPPCode(code, NameLoc);

  return ParseStatementOrDeclaration(Stmts, OnlyStatement);
}

// scout - parse a camera declaration
// return true on success
// these look like:

//camera cam {
//  near = 70.0;
//  far = 500.0;
//  fov = 40.0;
//  pos = float3(350.0, -100.0, 650.0);
//  lookat = float3(350.0, 200.0, 25.0);
//  up = float3(-1.0, 0.0, 0.0);
//};


StmtResult
Parser::ParseCameraDeclaration(StmtVector &Stmts,
                               bool OnlyStatement){
  ConsumeToken();

  if(Tok.isNot(tok::identifier)){
    Diag(Tok, diag::err_expected_ident);
    SkipUntil(tok::semi);
    ConsumeToken();
    return StmtError();
  }

  IdentifierInfo* Name = Tok.getIdentifierInfo();
  SourceLocation NameLoc = ConsumeToken();

  if(Tok.isNot(tok::l_brace)){
    Diag(Tok, diag::err_expected_lbrace);

    SkipUntil(tok::r_brace);
    SkipUntil(tok::semi);
    ConsumeToken();
    return StmtError();
  }

  ConsumeBrace();

  typedef std::map<std::string, Expr*> ArgExprMap;

  ArgExprMap argExprMap;

  bool error = false;

  for(;;){
    if(Tok.is(tok::r_brace) || Tok.is(tok::eof)){
      break;
    }

    if(Tok.isNot(tok::identifier)){
      Diag(Tok, diag::err_expected_ident);

      SkipUntil(tok::r_brace);
      SkipUntil(tok::semi);
      ConsumeToken();
      return StmtError();
    }

    IdentifierInfo* Arg = Tok.getIdentifierInfo();
    SourceLocation ArgLoc = ConsumeToken();
    (void)ArgLoc; //suppress warning

    if(Tok.isNot(tok::equal)){
      Diag(Tok, diag::err_expected_equal_after) << Arg->getName();

      SkipUntil(tok::r_brace);
      SkipUntil(tok::semi);
      ConsumeToken();
      return StmtError();
    }

    ConsumeToken();

    ExprResult argResult = ParseExpression();
    if(argResult.isInvalid()){
      error = true;
    }

    argExprMap[Arg->getName()] = argResult.get();

    if(Tok.isNot(tok::semi)){
      Diag(Tok, diag::err_expected_semi_camera_arg);

      SkipUntil(tok::r_brace);
      SkipUntil(tok::semi);
      ConsumeToken();
      return StmtError();
    }

    ConsumeToken();
  }

  assert(Tok.is(tok::r_brace) && "expected r_brace");

  ConsumeBrace();

  assert(Tok.is(tok::semi) && "expected semi");

  ConsumeToken();

  if(error){
    return StmtError();
  }

  std::string code;

  code = "scout::glCamera " + Name->getName().str() + "(";

  // put together the constructor call for glCamera

  ArgExprMap::iterator itr = argExprMap.find("fov");
  if(itr == argExprMap.end()){
    Diag(Tok, diag::err_missing_field_window_decl) << "fov";
    error = true;
  }

  code += ToCPPCode(itr->second);
  code += ",";

  itr = argExprMap.find("pos");
  if(itr == argExprMap.end()){
    Diag(Tok, diag::err_missing_field_window_decl) << "pos";
    error = true;
  }

  code += "scout::glfloat3(";
  code += ToCPPCode(itr->second) + ".x, ";
  code += ToCPPCode(itr->second) + ".y, ";
  code += ToCPPCode(itr->second) + ".z ";
  code += "), ";

  itr = argExprMap.find("lookat");
  if(itr == argExprMap.end()){
    Diag(Tok, diag::err_missing_field_window_decl) << "lookat";
    error = true;
  }

  code += "scout::glfloat3(";
  code += ToCPPCode(itr->second) + ".x, ";
  code += ToCPPCode(itr->second) + ".y, ";
  code += ToCPPCode(itr->second) + ".z ";
  code += "), ";

  itr = argExprMap.find("up");
  if(itr == argExprMap.end()){
    Diag(Tok, diag::err_missing_field_window_decl) << "up";
    error = true;
  }

  code += "scout::glfloat3(";
  code += ToCPPCode(itr->second) + ".x, ";
  code += ToCPPCode(itr->second) + ".y, ";
  code += ToCPPCode(itr->second) + ".z ";
  code += "), ";

  itr = argExprMap.find("near");
  if(itr == argExprMap.end()){
    Diag(Tok, diag::err_missing_field_window_decl) << "near";
    error = true;
  }

  code += ToCPPCode(itr->second);
  code += ", ";

  itr = argExprMap.find("far");
  if(itr == argExprMap.end()){
    Diag(Tok, diag::err_missing_field_window_decl) << "far";
    error = true;
  }

  code += ToCPPCode(itr->second);
  code += ");";

  if(error){
    return StmtError();
  }

  InsertCPPCode(code, NameLoc);

  return ParseStatementOrDeclaration(Stmts, OnlyStatement);
}
