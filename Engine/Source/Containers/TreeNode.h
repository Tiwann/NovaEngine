#pragma once
#include "Containers/Function.h"
#include "Containers/Array.h"
#include <optional>

namespace Nova
{
    template<class TreeNodeType>
    class TreeNode
    {
    public:
        using TreeNodeArray = Array<TreeNode>;
        using TreeNodeIterator = typename TreeNodeArray::Iterator;
        using TreeNodeConstIterator = typename TreeNodeArray::ConstIterator;
        
        TreeNode() = default;
        TreeNode(const TreeNodeType& Value) : m_Value(Value){}

        TreeNode& AddChild(const TreeNodeType& Child)
        {
            m_Children.Add({Child});
            TreeNode& ChildNode = m_Children.Last();
            ChildNode.m_Parent = this;
            return ChildNode;
        }

        void ForEach(Function<void(const TreeNode& Node)> Delegate) const
        {
            for (const auto & Child : m_Children)
                Delegate(Child);
        }
        
        bool HasChildren() const { return !m_Children.IsEmpty(); }
        bool HasParent() const { return m_Parent; }
        TreeNode& GetParent() const { return *m_Parent; }
        TreeNodeArray& GetChildren() const { return m_Children; }

        bool IsRoot() const { return !HasParent() && !m_Value.has_value(); }

        const TreeNodeType& GetValue() const { return m_Value.value(); }
             
        TreeNodeIterator begin() { return m_Children.begin(); }
        TreeNodeIterator end() { return m_Children.end(); }
        TreeNodeConstIterator begin() const { return m_Children.begin(); }
        TreeNodeConstIterator end() const { return m_Children.end(); }

    private:
        std::optional<TreeNodeType> m_Value;
        TreeNode* m_Parent = nullptr;
        TreeNodeArray m_Children;
    };
}
