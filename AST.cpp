enum ASTNodeType 
{
   Import,
   Package,
   OpenBracket,
   CloseBracket,
   Identifier,
   EndofText,
   StringLiteral
};
 
class ASTNode
{
public:
   ASTNodeType Type;
   char     Value[8192];
   ASTNode*    Left;
   ASTNode*    Right;
 
   ASTNode()
   {
      Type = Undefined;
      Value = 0;
      Left = NULL;
      Right = NULL;
   }
 
   ~ASTNode()
   {
      delete Left;
      delete Right;
   }
};

class Parser
{
   Token m_crtToken;
   const char* m_Text;
   size_t m_Index;
 
private:
 
   ASTNode* Expression()
   {
      ASTNode* tnode = Term();
      ASTNode* e1node = Expression1();
 
      return CreateNode(Package, tnode, e1node);
   }
 
   ASTNode* Expression1()
   {
      ASTNode* tnode;
      ASTNode* e1node;
 
      switch(m_crtToken.Type)
      {
      case Package:
         GetNextToken();
         tnode = Term();
         e1node = Expression1();
 
         return CreateNode(Package, e1node, tnode);
 
      case Import:
         GetNextToken();
         tnode = Term();
         e1node = Expression1();
 
         return CreateNode(Import, e1node, tnode);
      }
 
      return CreateNodeStr(0);
   }
 
   // ASTNode* Term()
   // {
   //    ASTNode* fnode = Factor();
   //    ASTNode* t1node = Term1();
 
   //    return CreateNode(OperatorMul, fnode, t1node);
   // }
 
   // ASTNode* Term1()
   // {
   //    ASTNode* fnode;
   //    ASTNode* t1node;
 
   //    switch(m_crtToken.Type)
   //    {
   //    case Mul: 
   //       GetNextToken();
   //       fnode = Factor();
   //       t1node = Term1();
   //       return CreateNode(OperatorMul, t1node, fnode);
 
   //    case Div:
   //       GetNextToken();
   //       fnode = Factor();
   //       t1node = Term1();
   //       return CreateNode(OperatorDiv, t1node, fnode);
   //    }
 
   //    return CreateNodeNumber(1);
   // }
 
   // ASTNode* Factor()
   // {
   //    ASTNode* node;
   //    switch(m_crtToken.Type)
   //    {
   //    case OpenParenthesis:
   //       GetNextToken();
   //       node = Expression();
   //       Match(')');
   //       return node;
 
   //    case Minus:
   //       GetNextToken();
   //     node = Factor();
   //       return CreateUnaryNode(node);
 
      // case Number:
      //    {
      //       double value = m_crtToken.Value;
      //       GetNextToken();
      //       return CreateNodeNumber(value);
      //    }
 
      // default:
      //    {
      //       std::stringstream sstr; 
      //       sstr << "Unexpected token '" << m_crtToken.Symbol << "' at position " << m_Index;
      //       throw ParserException(sstr.str(), m_Index);
      //    }
//       }
// }
 
   ASTNode* CreateNode(ASTNodeType type, ASTNode* left, ASTNode* right)
   {
      ASTNode* node = new ASTNode;
      node->Type = type;
      node->Left = left;
      node->Right = right;
 
      return node;
   }
 
   // ASTNode* CreateUnaryNode(ASTNode* left)
   // {
   //    ASTNode* node = new ASTNode;
   //    node->Type = UnaryMinus;
   //    node->Left = left;
   //    node->Right = NULL;
 
   //    return node;
   // }
 
   ASTNode* CreateNodeStr(char value[8192])
   {
      ASTNode* node = new ASTNode;
      node->Type = StringLiteral;
      node->Value = value;
 
      return node;
   }
 
   void Match(char expected)
   {
      if(m_Text[m_Index-1] == expected)
         GetNextToken();
      else
      {
         std::stringstream sstr;
         sstr << "Expected token '" << expected << "' at position " << m_Index;
         throw ParserException(sstr.str(), m_Index);
      }
   }
 
   void SkipWhitespaces()
   {
      while(isspace(m_Text[m_Index])) m_Index++;
   }
 
   void GetNextToken()
   {
      SkipWhitespaces();
 
     m_crtToken.Value = 0;
     m_crtToken.Symbol = 0;
 
      if(m_Text[m_Index] == 0)
      {
         m_crtToken.Type = EndOfText;
         return;
      }
 
      // if(isdigit(m_Text[m_Index]))
      // {
      //    m_crtToken.Type = Identifier;
      //  m_crtToken.Value = GetNumber();
      //    return;
      // }
 
      m_crtToken.Type = Error;
 
      switch(m_Text[m_Index])
      {
      case '(': 
         m_crtToken.Type = OpenBracket; 
         break;
      case ')': 
         m_crtToken.Type = CloseBracket; 
         break;
      case 'i': 
         m_crtToken.Type = Import; 
         break;
      case 'p': 
         m_crtToken.Type = Package; 
         break;
      default:
         m_crtToken.Type = Identifier; 
      }
 
      if(m_crtToken.Type != Error)
     {
         m_crtToken.Symbol = m_Text[m_Index];
         m_Index++;
     }
      else
      {
         std::stringstream sstr; 
         sstr << "Unexpected token '" << m_Text[m_Index] << "' at position " << m_Index;
         throw ParserException(sstr.str(), m_Index);
      }
   }
 
   // double GetNumber()
   // {
   //    SkipWhitespaces();
 
   //    int index = m_Index;
   //    while(isdigit(m_Text[m_Index])) m_Index++;
   //    if(m_Text[m_Index] == '.') m_Index++;
   //    while(isdigit(m_Text[m_Index])) m_Index++;
 
   //    if(m_Index - index == 0)
   //       throw ParserException("Number expected but not found!", m_Index);
 
   //    char buffer[32] = {0};
   //    memcpy(buffer, &m_Text[index], m_Index - index);
 
   //    return atof(buffer);
   // }
 
public:
   ASTNode* Parse(const char* text)
   {
      m_Text = text;
      m_Index = 0;
      GetNextToken();
 
      return Expression();
   }
};
