import unittest

from audit import normalize_constraint, normalize_declaration, signature_shape, split_declarations


class AuditTests(unittest.TestCase):
    def test_normalizes_template_names_and_namespace(self):
        left = "template<class T> constexpr ftl::vector<T> f(T value);"
        right = "template<typename U> constexpr std::vector<U> f(U value);"
        self.assertEqual(normalize_declaration(left), normalize_declaration(right))

    def test_normalizes_safe_conjunction(self):
        self.assertEqual(normalize_constraint("(B<T> && A<T>)"), normalize_constraint("A<T>&&B<T>"))

    def test_split_ignores_nested_semicolons(self):
        block = "template<class T> struct X { void f(); };\nvoid g(int);"
        self.assertEqual(len(split_declarations(block)), 3)

    def test_signature_shape_preserves_structure_not_names(self):
        self.assertEqual(signature_shape("f(const T&, U*)"), signature_shape("g(const X&, Y*)"))
        self.assertNotEqual(signature_shape("f(T&)"), signature_shape("f(T*)"))


if __name__ == "__main__":
    unittest.main()
