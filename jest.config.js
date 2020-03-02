module.exports = {
  testEnvironment: 'node',
  transform: {
    '^.+\\.tsx?$': 'ts-jest',
  },
  testMatch: ['**/__tests__/**/*.ts?(x)', '**/?(*.)+(spec|test).ts?(x)'],
  moduleFileExtensions: ['ts', 'tsx', 'js', 'jsx', 'node', 'json'],
  collectCoverage: false,
  collectCoverageFrom: ['src/**/*.{ts,tsx}'],
  coveragePathIgnorePatterns: ['/node_modules/', '\\.d\\.ts$', '\\.(spec|test)\\.ts$'],
  globals: {
    'ts-jest': {
      tsConfig: './tsconfig.json',
    },
  },
  moduleNameMapper: {
    '^@/(.+)': '<rootDir>/src/$1',
  },
}
